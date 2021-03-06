/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/logic/luadefs/CLuaCryptDefs.cpp
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
#include "StdInc.h"
#include <SharedUtil.Crypto.h>

void CLuaCryptDefs::LoadFunctions(void)
{
    std::map<const char*, lua_CFunction> functions{
        {"md5", Md5},
        {"sha256", Sha256},
        {"hash", Hash},
        {"teaEncode", TeaEncode},
        {"teaDecode", TeaDecode},
        {"base64Encode", Base64encode},
        {"base64Decode", Base64decode},
        {"passwordHash", PasswordHash},
        {"passwordVerify", PasswordVerify},
        {"encodeString", EncodeString},
        {"decodeString", DecodeString},
    };

    // Add functions
    for (const auto& pair : functions)
    {
        CLuaCFunctions::AddFunction(pair.first, pair.second);
    }
}

int CLuaCryptDefs::Md5(lua_State* luaVM)
{
    SString          strMd5 = "";
    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strMd5);

    if (!argStream.HasErrors())
    {
        MD5        md5bytes;
        char       szResult[33];
        CMD5Hasher hasher;
        hasher.Calculate(strMd5, strMd5.length(), md5bytes);
        hasher.ConvertToHex(md5bytes, szResult);
        lua_pushstring(luaVM, szResult);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaCryptDefs::Sha256(lua_State* luaVM)
{
    //  string sha256 ( string str )
    SString strSourceData;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strSourceData);

    if (!argStream.HasErrors())
    {
        SString strResult = GenerateSha256HexString(strSourceData);
        lua_pushstring(luaVM, strResult);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaCryptDefs::Hash(lua_State* luaVM)
{
    //  string hash ( string type, string data )
    EHashFunctionType hashFunction;
    SString           strSourceData;

    CScriptArgReader argStream(luaVM);
    argStream.ReadEnumString(hashFunction);
    argStream.ReadString(strSourceData);

    if (!argStream.HasErrors())
    {
        SString strResult = GenerateHashHexString(hashFunction, strSourceData);
        lua_pushstring(luaVM, strResult.ToLower());
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaCryptDefs::TeaEncode(lua_State* luaVM)
{
    SString str;
    SString key;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(str);
    argStream.ReadString(key);

    if (!argStream.HasErrors())
    {
        SString result;
        SString humanReadableResult;
        SharedUtil::TeaEncode(str, key, &result);
        humanReadableResult = SharedUtil::Base64encode(result);
        lua_pushstring(luaVM, humanReadableResult);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaCryptDefs::TeaDecode(lua_State* luaVM)
{
    SString str;
    SString key;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(str);
    argStream.ReadString(key);

    if (!argStream.HasErrors())
    {
        SString result = SharedUtil::Base64decode(str);
        SharedUtil::TeaDecode(result, key, &str);
        lua_pushstring(luaVM, str);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaCryptDefs::Base64encode(lua_State* luaVM)
{
    SString str;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(str);

    if (!argStream.HasErrors())
    {
        lua_pushstring(luaVM, SharedUtil::Base64encode(str));
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaCryptDefs::Base64decode(lua_State* luaVM)
{
    SString str;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(str);

    if (!argStream.HasErrors())
    {
        SString result = SharedUtil::Base64decode(str);
        lua_pushlstring(luaVM, result, result.length());
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaCryptDefs::PasswordHash(lua_State* luaVM)
{
    //  string password_hash(string password, string algorithm, table options = {} [, function callback])
    SString              password;
    PasswordHashFunction algorithm;
    CStringMap           options;
    CLuaFunctionRef      luaFunctionRef;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(password);
    argStream.ReadEnumString(algorithm);
    argStream.ReadStringMap(options);

    if (argStream.NextIsFunction())
    {
        argStream.ReadFunction(luaFunctionRef);
        argStream.ReadFunctionComplete();
    }

    if (!argStream.HasErrors())
    {
        if (algorithm == PasswordHashFunction::Bcrypt)
        {
            // Set default value to 10
            if (options["cost"].empty())
                options["cost"] = "10";

            std::stringstream ss(options["cost"]);
            std::size_t       cost;
            ss >> cost;

            if (!ss.fail())
            {
                // Sync
                if (luaFunctionRef == CLuaFunctionRef{})
                {
                    SString hash = SharedUtil::BcryptHash(password, options["salt"], cost);
                    if (!hash.empty())
                    {
                        lua_pushstring(luaVM, hash);
                        return 1;
                    }
                    else
                        m_pScriptDebugging->LogCustom(luaVM, "Invalid value for field 'salt'");
                }
                else            // Async
                {
                    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
                    if (pLuaMain)
                    {
                        CLuaShared::GetAsyncTaskScheduler()->PushTask<SString>(
                            [password, salt = options["salt"], cost] {
                                // Execute time-consuming task
                                return SharedUtil::BcryptHash(password, salt, cost);
                            },
                            [luaFunctionRef](const SString& hash) {
                                CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaFunctionRef.GetLuaVM());
                                if (pLuaMain)
                                {
                                    CLuaArguments arguments;

                                    if (hash.empty())
                                    {
                                        m_pScriptDebugging->LogCustom(pLuaMain->GetVM(), "Invalid value for field 'salt'");
                                        arguments.PushBoolean(false);
                                    }
                                    else
                                        arguments.PushString(hash);

                                    arguments.Call(pLuaMain, luaFunctionRef);
                                }
                            });

                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                }
            }
            else
                m_pScriptDebugging->LogWarning(luaVM, "Invalid value for field 'cost'");
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaCryptDefs::PasswordVerify(lua_State* luaVM)
{
    //  bool passwordVerify(string password, string hash [, table options, function callback])
    SString         password;
    SString         hash;
    CStringMap      options;
    CLuaFunctionRef luaFunctionRef;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(password);
    argStream.ReadString(hash);

    if (argStream.NextIsTable())
    {
        argStream.ReadStringMap(options);
    }

    if (argStream.NextIsFunction())
    {
        argStream.ReadFunction(luaFunctionRef);
        argStream.ReadFunctionComplete();
    }

    if (!argStream.HasErrors())
    {
        // Is it bcrypt?
        if (hash.BeginsWith("$2y$") || hash.BeginsWith("$2a$"))
        {
            bool insecureBcrypt;
            options.ReadBool("insecureBcrypt", insecureBcrypt, false);

            if (hash.BeginsWith("$2a$") && !insecureBcrypt)
            {
                m_pScriptDebugging->LogWarning(luaVM, "Bcrypt 2a prefix is only supported if the 'insecureBcrypt' option is enabled. Consider upgrading to 2y");
                lua_pushnil(luaVM);
                return 1;
            }

            // Sync
            if (luaFunctionRef == CLuaFunctionRef())
            {
                lua_pushboolean(luaVM, SharedUtil::BcryptVerify(password, hash));
            }
            else            // Async
            {
                CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
                if (pLuaMain)
                {
                    CLuaShared::GetAsyncTaskScheduler()->PushTask<bool>(
                        [password, hash] {
                            // Execute time-consuming task
                            return SharedUtil::BcryptVerify(password, hash);
                        },
                        [luaFunctionRef](const bool& correct) {
                            CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaFunctionRef.GetLuaVM());
                            if (pLuaMain)
                            {
                                CLuaArguments arguments;
                                arguments.PushBoolean(correct);
                                arguments.Call(pLuaMain, luaFunctionRef);
                            }
                        });

                    lua_pushboolean(luaVM, true);
                }
            }
            return 1;
        }
        else
        {
            m_pScriptDebugging->LogWarning(luaVM, "Passed unknown hash");
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushnil(luaVM);
    return 1;
}

int CLuaCryptDefs::EncodeString(lua_State* luaVM)
{
    StringEncryptFunction algorithm;
    SString               data;
    CStringMap            options;

    CScriptArgReader argStream(luaVM);
    argStream.ReadEnumString(algorithm);
    argStream.ReadString(data);
    argStream.ReadStringMap(options);

    if (!argStream.HasErrors())
    {
        switch (algorithm)
        {
            case StringEncryptFunction::TEA:
            {
                SString& key = options["key"];

                if (key.empty())
                {
                    m_pScriptDebugging->LogCustom(luaVM, "Invalid value for field 'key'");
                    lua_pushboolean(luaVM, false);
                    return 1;
                }

                SString result;
                SharedUtil::TeaEncode(data, key, &result);
                lua_pushlstring(luaVM, result, result.length());
                return 1;
            }
            default:
            {
                m_pScriptDebugging->LogCustom(luaVM, "Unknown encryption algorithm");
                lua_pushboolean(luaVM, false);
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaCryptDefs::DecodeString(lua_State* luaVM)
{
    StringEncryptFunction algorithm;
    SString               data;
    CStringMap            options;

    CScriptArgReader argStream(luaVM);
    argStream.ReadEnumString(algorithm);
    argStream.ReadString(data);
    argStream.ReadStringMap(options);

    if (!argStream.HasErrors())
    {
        switch (algorithm)
        {
            case StringEncryptFunction::TEA:
            {
                SString& key = options["key"];

                if (key.empty())
                {
                    m_pScriptDebugging->LogCustom(luaVM, "Invalid value for field 'key'");
                    lua_pushboolean(luaVM, false);
                    return 1;
                }

                SString result;
                SharedUtil::TeaDecode(data, key, &result);
                lua_pushlstring(luaVM, result, result.length());
                return 1;
            }
            default:
            {
                m_pScriptDebugging->LogCustom(luaVM, "Unknown encryption algorithm");
                lua_pushboolean(luaVM, false);
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

/**
 * Вызов функций из других плагинов для SourceMod 1.10+
 * 
 * https://github.com/deathscore13/call_publics.inc
 */

#if defined _call_publics_included
 #endinput
#endif
#define _call_publics_included


stock static int iPtr[1];
stock static Function iFunc;

/**
 * Бывший memset. Параметр offset убран из соображений безопасности .data секции
 */
stock static void setFunc(int[] ptr, int data)
{
    ptr[1] = data;
}

stock static Function GetFunctionById(int id)
{
    setFunc(iPtr, id);
    return iFunc;
}


methodmap CallPublics __nullable__
{
    /**
     * Создание объекта CallPublics
     * Закрывать только через метод Close()!
     * 
     * @param gd            Объект GameData
     * @param plugin        Дескриптор плагина
     * 
     * @return              Объект CallPublics
     * @error               Плагин не найден
     */
    public CallPublics(GameData gd, Handle plugin = INVALID_HANDLE)
    {
        DataPack pack = new DataPack();
        pack.WriteCell(CloneHandle(gd));

        if (plugin == INVALID_HANDLE)
        {
            char buffer[PLATFORM_MAX_PATH];
            gd.GetKeyValue("plugin", buffer, sizeof(buffer));
            plugin = FindPluginByFile(buffer);

            if (plugin == INVALID_HANDLE)
                ThrowError("CallPublics: plugin '%s' not found", buffer);
        }
        pack.WriteCell(plugin);

        return view_as<CallPublics>(pack);
    }

    /**
     * Добавляет функцию в список вызовов приватного форварда
     * 
     * @param fwd           Объект PrivateForward
     * @param key           Ключ к имени/идентификатору функции из объекта GameData
     * 
     * @return              true если добавлено, false если нет
     * @error               Ключ или функция не найдена
     */
    public bool AddFunction(PrivateForward fwd, const char[] key)
    {
        view_as<DataPack>(this).Reset();

        char buffer[256];
        if (!view_as<GameData>(view_as<DataPack>(this).ReadCell()).GetKeyValue(key, buffer, sizeof(buffer)))
            ThrowError("CallPublics::GetFunction: key '%s' not found", key);
        
        if (buffer[0] == '@')
        {
            Handle plugin = view_as<DataPack>(this).ReadCell();
            Function func = GetFunctionByName(plugin, buffer[1]);
            
            if (func == INVALID_FUNCTION)
                ThrowError("CallPublics::GetFunction: function '%s' not found", buffer[1]);

            return fwd.AddFunction(plugin, func);
        }
        return fwd.AddFunction(view_as<DataPack>(this).ReadCell(), GetFunctionById(StringToInt(buffer)));
    }

    /**
     * Закрытие объекта CallPublics
     * Не используйте CloseHandle()!
     */
    public void Close()
    {
        view_as<DataPack>(this).Reset();

        view_as<GameData>(view_as<DataPack>(this).ReadCell()).Close();
        view_as<DataPack>(this).Close();
    }
}

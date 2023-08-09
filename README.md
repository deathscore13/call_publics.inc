# call_publics.inc
### Вызов функций из других плагинов для SourceMod 1.10+<br><br>

Небольшой помощник по вызову функций из других плагинов<br><br>
Имена функций должны начинаться с `@`, как имена сигнатур<br><br>
Откуда брать имена/идентификаторы функций и другая полезная информация в [**ЭТОЙ**](https://hlmod.net/articles/63) статье

<br><br>
## Установка
1. Переместить файл **`call_publics.inc`** в папку **`include`**

<br><br>
## Пример использования
**`test.sp`**:
```sp
#include <sourcemod>

public void OnPluginStart()
{
    func("test");
}

int func(const char[] text)
{
    PrintToServer(text);
    return 123;
}
```
**`Path_SM/gamedata/test.txt`**:
```kv
"Games"
{
    "*"
    {
        "Keys"
        {
            // если в плагине не указан дескриптор целевого плагина, то значение берётся из ключа "plugin"
            "plugin"    "test.smx"

            // имя функции из секции .publics (вместо имени можно использовать идентификатор)
            "func"      "@.3032.func"
        }
    }
}
```
**`main.sp`**:
```sp
#include <sourcemod>

// подключение call_publics.inc
#include <call_publics>

// глобальная переменная для возможности вызова функции из любой части кода
PrivateForward hFwd;

// макрос для удобства вызова функции
#define FUNC(%0,%1) \
    Call_StartForward(hFwd); \
    Call_PushString(%1); \
    Call_Finish(%0)

public void OnPluginStart()
{
    // создание прототипа функции
    hFwd = new PrivateForward(ET_Single, Param_String);

    // создание объекта CallPublics с данными из Path_SM/gamedata/test.txt
    CallPublics hndl = new CallPublics(new GameData("test"));

    // добавление функции из CallPublics в hFwd
    hndl.AddFunction(hFwd, "func");

    // больше нигде не используется, удаляем (не используйте CloseHandle()!)
    hndl.Close();

    // создание переменной под ответ
    int ret;

    // вызов функции
    FUNC(ret, "main");

    // вывод ответа
    PrintToServer("ret = %d", ret);
}
```

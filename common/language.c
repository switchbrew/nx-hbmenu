#include "language.h"

#ifdef __SWITCH__
#define STR_JP(_str) [SetLanguage_JA] = _str
#define STR_EN(_str) [SetLanguage_ENUS] = _str, [SetLanguage_ENGB] = _str
#define STR_FR(_str) [SetLanguage_FR] = _str, [SetLanguage_FRCA] = _str
#define STR_DE(_str) [SetLanguage_DE] = _str
#define STR_IT(_str) [SetLanguage_IT] = _str
#define STR_ES(_str) [SetLanguage_ES] = _str, [SetLanguage_ES419] = _str
#define STR_ZH_HANS(_str) [SetLanguage_ZHCN] = _str, [SetLanguage_ZHHANS] = _str
#define STR_KO(_str) [SetLanguage_KO] = _str
#define STR_NL(_str) [SetLanguage_NL] = _str
#define STR_PT(_str) [SetLanguage_PT] = _str
#define STR_RU(_str) [SetLanguage_RU] = _str
#define STR_ZH_HANT(_str) [SetLanguage_ZHTW] = _str, [SetLanguage_ZHHANT] = _str
#else
#define STR_JP(_str) [0] = _str
#define STR_EN(_str) [1] = _str
#define STR_FR(_str) [2] = _str
#define STR_DE(_str) [3] = _str
#define STR_IT(_str) [4] = _str
#define STR_ES(_str) [5] = _str
#define STR_ZH_HANS(_str) [6] = _str
#define STR_KO(_str) [7] = _str
#define STR_NL(_str) [8] = _str
#define STR_PT(_str) [9] = _str
#define STR_RU(_str) [10] = _str
#define STR_ZH_HANT(_str) [11] = _str
#endif

const char* const g_strings[StrId_Max][17] =
{
    [StrId_Loading] =
    {
        STR_EN("Loading…"),
        STR_ES("Cargando…"),
        STR_DE("Lade…"),
        STR_FR("Chargement…"),
        STR_IT("Caricamento…"),
        STR_JP("ロード中…"),
        STR_PT("Carregando…"),
        STR_NL("Laden…"),
        STR_KO("로딩 중…"),
        STR_RU("Загрузка…"),
        STR_ZH_HANS("加载中…"),
        STR_ZH_HANT("載入中…"),
    },

    [StrId_AppletMode] =
    {
        STR_EN("● Applet Mode ●"),
        STR_ES("● Modo Applet ●"),
        STR_FR("● Mode Applet ●"),
        STR_KO("● 애플릿 모드 ●"),
        STR_ZH_HANS("● 小程序模式 ●"),
        STR_RU("● Режим Applet ●"),
    },

    [StrId_Directory] =
    {
        STR_EN("Directory"),
        STR_ES("Carpeta"),
        STR_DE("Verzeichnis"),
        STR_FR("Dossier"),
        STR_IT("Cartella"),
        STR_JP("フォルダ"),
        STR_PT("Directório"),
        STR_NL("Map"),
        STR_KO("디렉토리"),
        STR_RU("Директория"),
        STR_ZH_HANS("文件夹"),
        STR_ZH_HANT("資料夾"),
    },

    /*[StrId_DefaultLongTitle] =
    {
        STR_EN("Homebrew application"),
        STR_ES("Aplicación homebrew"),
        STR_DE("Homebrew-Anwendung"),
        STR_FR("Application homebrew"),
        STR_IT("Applicazione homebrew"),
        STR_JP("自作アプリ"),
        STR_PT("Aplicação Homebrew"),
        STR_NL("Homebrew toepassing"),
        STR_KO("홈브류 애플리케이션"),
        STR_RU("приложение хомебреw"),
        STR_ZH_HANS("自制应用程序"),
        STR_ZH_HANT("自製程式"),
    },*/

    [StrId_DefaultPublisher] =
    {
        STR_EN("Unknown author"),
        STR_ES("Autor desconocido"),
        STR_DE("Unbekannter Autor"),
        STR_FR("Auteur inconnu"),
        STR_IT("Autore sconosciuto"),
        STR_JP("作者不明"),
        STR_PT("Autor Desconhecido"),
        STR_NL("Auteur onbekend"),
        STR_KO("알 수 없는 개발자"),
        STR_RU("Неизвестный автор"),
        STR_ZH_HANS("未知作者"),
        STR_ZH_HANT("作者未知"),
    },

    [StrId_IOError] =
    {
        STR_EN("I/O Error"),
        STR_ES("Error de E/S"),
        STR_DE("I/O-Fehler"),
        STR_FR("Erreur d'E/S"),
        STR_IT("Errore I/O"),
        STR_JP("入出力エラー"),
        STR_PT("Erro de E/S"),
        STR_NL("I/O Fout"),
        STR_KO("입출력 오류"),
        STR_RU("Ошибка ввода/вывода"),
        STR_ZH_HANS("读写出错"),
        STR_ZH_HANT("取存錯誤"),
    },

    [StrId_CouldNotOpenFile] =
    {
        STR_EN("Could not open file:\n%s"),
        STR_ES("No se pudo abrir el archivo:\n%s"),
        STR_DE("Konnte Datei nicht öffnen:\n%s"),
        STR_FR("Impossible d'ouvrir le fichier :\n%s"),
        STR_IT("Impossibile aprire il file:\n%s"),
        STR_JP("ファイルを開くことができませんでした：\n%s"),
        STR_PT("Não foi possível abrir o ficheiro:\n%s"),
        STR_NL("Kan bestand niet openen:\n%s"),
        STR_KO("파일을 열 수 없습니다:\n%s"),
        STR_RU("Невозможно открыть файл:\n%s"),
        STR_ZH_HANS("无法打开文件:\n%s"),
        STR_ZH_HANT("無法開啟檔案:\n%s"),
    },

    [StrId_NroNotFound] =
    {
        STR_EN("Could not find executable: %s"),
        STR_FR("Impossible trouver l'exécutable : %s"),
        STR_KO("실행 파일을 찾을 수 없습니다: %s"),
        STR_ZH_HANS("找不到可执行文件: %s"),
        STR_ZH_HANT("沒有可執行檔案: %s"),
        STR_RU("Не удалось найти исполняемый файл: %s"),
    },

    [StrId_NoAppsFound_Title] =
    {
        STR_EN("No applications found"),
        STR_ES("No hay aplicaciones"),
        STR_DE("Keine Anwendungen gefunden"),
        STR_FR("Aucune application trouvée"),
        STR_IT("Nessun'applicazione trovata"),
        STR_JP("アプリが見つかりませんでした"),
        STR_PT("Não foram encontradas aplicações"),
        STR_NL("Geen toepassingen gevonden"),
        STR_KO("애플리케이션을 찾을 수 없습니다"),
        STR_RU("Приложения не найдены"),
        STR_ZH_HANS("找不到可执行的自制程序"),
        STR_ZH_HANT("沒有可執行的自製程式"),
    },

    [StrId_NoAppsFound_Msg] =
    {
        STR_EN(
            "No applications could be found on the SD card.\n"
            "Make sure a folder named /switch exists in the\n"
            "root of the SD card and it contains applications.\n"
        ),
        STR_ES(
            "No se han podido encontrar aplicaciones en la\n"
            "tarjeta SD. Compruebe que haya una carpeta\n"
            "llamada /switch y que contenga aplicaciones.\n"
        ),
        STR_DE(
            "Auf der SD-Karte wurden keine Anwendungen\n"
            "gefunden. Stelle sicher, dass ein Verzeichnis\n"
            "namens /switch im Hauptverzeichnis der SD-Karte\n"
            "existiert und Anwendungen enthält!"
        ),
        STR_FR(
            "Aucune application n'a été trouvée sur la carte\n"
            "SD. Veillez à ce qu'un dossier intitulé /switch\n"
            "existe à la racine de la carte SD et à ce qu'il\n"
            "contienne des applications."
        ),
        STR_IT(
            "Nessun'applicazione è stata trovata sulla scheda\n"
            "SD. Assicurati che esista una cartella chiamata\n"
            "/switch nella root della scheda SD e che contenga\n"
            "delle applicazioni."
        ),
        STR_JP(
            "SDカードにアプリケーションが見つかりませんでした。\n"
            "SDカードのルートに「switch」という名前のフォルダを\n"
            "作成してください。"
        ),
        STR_KO(
            "SD 카드에서 애플리케이션을 찾을 수 없습니다.\n"
            "SD 카드 최상위에 /switch 폴더가 있고\n"
            "애플리케이션이 포함되어 있는지 확인하세요."
        ),
        STR_PT(
            "Nenhuma aplicação foi encontrada no cartão SD.\n"
            "Certifique-se que uma pasta com o nome /switch\n"
            "existe na raiz do cartão SD e que contêm\n"
            "aplicações."
        ),
        STR_NL(
            "Geen toepassingen gevonden op de SD kaart.\n"
            "Zorg ervoor dat een map genaamd /switch in de\n"
            "rootdirectory van de SD kaart aangemaakt is\n"
            "en de toepassingen bevat."
        ),
        STR_RU(
            "На SD-карте не удалось найти ни одного приложения.\n"
            "Убедитесь, что на SD-карте существует папка с именем switch в\n"
            "корневом каталоге и в ней содержатся приложения."
        ),
        STR_ZH_HANS(
            "找不到任何自制程序(nro)。\n"
            "请在SD卡根目录创建“switch”文件夹，\n"
            "并将自制程序(nro)放入其中。"
        ),
        STR_ZH_HANT(
            "記憶卡內沒有可供執行的應用程式。\n"
            "請在根目錄下建立「switch」資料夾，\n"
            "並將自製軟體複製到switch資料夾內。"
        ),
    },

    [StrId_LastLoadResult] =
    {
        STR_EN("The last application returned an error:"),
        STR_ES("La última aplicación devolvió un error:"),
        STR_DE("Die letzte Anwendung erzeugte einen Fehler:"),
        STR_FR("La dernière application a retourné une erreur:"),
        STR_IT("L'ultima applicazione ha restituito un errore:"),
        STR_JP("直前に実行したアプリでエラーが発生しました:"),
        STR_KO("최근 애플리케이션에서 오류가 발생했습니다:"),
        STR_ZH_HANS("程序运行后出现错误:"),
        STR_ZH_HANT("程式執行後出現錯誤:"),
        STR_RU("Последнее приложение вернуло ошибку:"),
    },

    [StrId_AppLaunchError] =
    {
        STR_EN("Failed to launch the application:"),
        STR_DE("Konnte die Anwendung nicht starten:"),
        STR_FR("Erreur au lancement de l'application:"),
        STR_IT("Errore nell'avvio dell'applicazione:"),
        STR_ES("No se ha podido iniciar la aplicación:"),
        STR_KO("애플리케이션을 시작하지 못했습니다:"),
        STR_ZH_HANS("运行程序时发生错误:"),
        STR_ZH_HANT("執行程式時發生錯誤:"),
        STR_RU("Не удалось запустить приложение:"),
    },

    [StrId_AppInfo_Author] =
    {
        STR_EN("Author"),
        STR_ES("Autor"),
        STR_DE("Autor"),
        STR_FR("Auteur"),
        STR_IT("Autore"),
        STR_JP("作者"),
        STR_KO("개발자"),
        STR_PT("Autor"),
        STR_NL("Auteur"),
        STR_RU("Автор"),
        STR_ZH_HANS("作者"),
        STR_ZH_HANT("作者"),
    },

    [StrId_AppInfo_Version] =
    {
        STR_EN("Version"),
        STR_ES("Versión"),
        STR_DE("Version"),
        STR_FR("Version"),
        STR_IT("Versione"),
        STR_JP("バージョン"),
        STR_KO("버전"),
        STR_PT("Versão"),
        STR_NL("Versie"),
        STR_RU("Версия"),
        STR_ZH_HANS("版本"),
        STR_ZH_HANT("版本"),
    },

    [StrId_Actions_Launch] =
    {
        STR_EN("Launch"),
        STR_ES("Lanzamiento"),
        STR_DE("Starten"),
        STR_FR("Lancer"),
        STR_IT("Avvia"),
        STR_JP("起動"),
        STR_KO("실행"),
        STR_PT("Lançamento"),
        STR_NL("Lancering"),
        STR_RU("Запустить"),
        STR_ZH_HANS("启动"),
        STR_ZH_HANT("啟動"),
    },

    [StrId_Actions_Open] =
    {
        STR_EN("Open"),
        STR_ES("Abrir"),
        STR_DE("Öffnen"),
        STR_FR("Ouvrir"),
        STR_IT("Apri"),
        STR_JP("開く"),
        STR_KO("열기"),
        STR_PT("Abrir"),
        STR_NL("Open"),
        STR_RU("Открыть"),
        STR_ZH_HANS("打开"),
        STR_ZH_HANT("開啟"),
    },

    [StrId_Actions_Back] =
    {
        STR_EN("Back"),
        STR_ES("Volver"),
        STR_DE("Zurück"),
        STR_FR("Retour"),
        STR_IT("Indietro"),
        STR_JP("戻る"),
        STR_KO("뒤로가기"),
        STR_PT("Regressar"),
        STR_NL("Terug"),
        STR_RU("Назад"),
        STR_ZH_HANS("返回"),
        STR_ZH_HANT("返回"),
    },

    [StrId_MsgBox_OK] =
    {
        STR_EN("OK"),
        STR_DE("OK"),
        STR_FR("OK"),
        STR_IT("OK"),
        STR_ES("Aceptar"),
        STR_JP("了解"),
        STR_KO("확인"),
        STR_ZH_HANS("确认"),
        STR_ZH_HANT("確認"),
        STR_RU("ОК"),
    },

    [StrId_Actions_Apply] =
    {
        STR_EN("Apply"),
        STR_FR("Appliquer"),
        STR_DE("Anwenden"),
        STR_ES("Aplicar"),
        STR_IT("Applica"),
        STR_JP("適用"),
        STR_KO("적용"),
        STR_ZH_HANS("应用"),
        STR_ZH_HANT("套用"),
        STR_RU("Применить"),
    },

    [StrId_Actions_Star] =
    {
        STR_EN("Star"),
        STR_ES("Agregar a favoritos"),
        STR_IT("Aggiungi ai preferiti"),
        STR_FR("Ajouter aux favoris"),
        STR_KO("즐겨찾기에 추가"),
        STR_ZH_HANS("收藏"),
        STR_RU("Добавить в избранное"),
    },

    [StrId_Actions_Unstar] =
    {
        STR_EN("Unstar"),
        STR_ES("Borrar de favoritos"),
        STR_IT("Rimuovi dai preferiti"),
        STR_FR("Retirer des favoris"),
        STR_KO("즐겨찾기에 삭제"),
        STR_ZH_HANS("取消收藏"),
        STR_RU("Удалить из избранного")
    },

    [StrId_ThemeMenu] =
    {
        STR_EN("Theme Menu"),
        STR_FR("Menu thèmes"),
        STR_DE("Theme Menü"),
        STR_ES("Menú temático"),
        STR_IT("Tema Menu"),
        STR_JP("テーマメニュー"),
        STR_KO("테마 메뉴"),
        STR_ZH_HANS("主题菜单"),
        STR_ZH_HANT("主題選單"),
        STR_RU("Меню тем")
    },

    [StrId_ThemeNotApplied] =
    {
        STR_EN("Theme cannot be applied because an error occurred."),
        STR_DE("Das Theme konnte nicht geladen werden, da ein Fehler aufgetreten ist."),
        STR_FR("Le thème ne peut pas être appliqué car une erreur est survenue."),
        STR_ES("El tema no se pudo aplicar porque se ha producido un error."),
        STR_IT("Il tema non è stato applicato a causa di un errore."),
        STR_JP("エラーが発生したため、テーマを適用できませんでした。"),
        STR_KO("오류가 발생 했기 때문에 테마를 적용할 수 없습니다."),
        STR_ZH_HANS("由于发生错误, 无法应用主题。"),
        STR_ZH_HANT("出現錯誤，無法套用主題。"),
        STR_RU("Тема не может быть применена из-за возникшей ошибки."),
    },

    [StrId_DefaultThemeName] =
    {
        STR_EN("Default Theme"),
        STR_FR("Thème par défaut"),
        STR_DE("Standard Theme"),
        STR_IT("Tema di default"),
        STR_ES("Tema por defecto"),
        STR_KO("기본 테마"),
        STR_ZH_HANS("默认主题"),
        STR_ZH_HANT("預設主題"),
        STR_RU("Тема по умолчанию"),
    },

    /*[StrId_Reboot] =
    {
        STR_EN(
            "Returning to \xEE\x81\xB3HOME is not available.\n"
            "You're about to reboot your console.\n\n"
            "  \xEE\x80\x80 Reboot\n"
            "  \xEE\x80\x81 Cancel"
        ),
        STR_ES(
            "Volver a \xEE\x81\xB3HOME no está disponible.\n"
            "Está a punto de reiniciar su consola.\n\n"
            "  \xEE\x80\x80 Reiniciar\n"
            "  \xEE\x80\x81 Cancelar"
        ),
        STR_DE(
            "Rückkehr zu \xEE\x81\xB3HOME nicht verfügbar.\n"
            "Deine Konsole wird neu gestartet.\n\n"
            "  \xEE\x80\x80 Neu starten\n"
            "  \xEE\x80\x81 Abbrechen"
        ),
        STR_FR(
            "Retour au menu \xEE\x81\xB3HOME indisponible.\n"
            "Vous êtes sur le point de redémarrer\n"
            "votre console.\n\n"
            "  \xEE\x80\x80 Redémarrer\n"
            "  \xEE\x80\x81 Annuler"
        ),
        STR_IT(
            "Ritorno al menu \xEE\x81\xB3HOME non disponibile.\n"
            "Stai per riavviare la tua console.\n\n"
            "  \xEE\x80\x80 Riavvia\n"
            "  \xEE\x80\x81 Annulla"
        ),
        STR_JP(
            " \xEE\x81\xB3HOME に戻ることができませんでした。\n"
            "今すぐ本体を再起動してください。\n\n"
            "  \xEE\x80\x80 再起動\n"
            "  \xEE\x80\x81 キャンセル"
        ),
        STR_KO(
            "\xEE\x81\xB3HOME 으로 돌아갈 수 없습니다.\n"
            "콘솔을 재부팅할 것 입니다.\n\n"
            "  \xEE\x80\x80 재부팅\n"
            "  \xEE\x80\x81 취소"
        ),
        STR_PT(
            "Regressar para \xEE\x81\xB3HOME não está\n"
            "disponível. Está a reiniciar a sua consola.\n\n"
            "  \xEE\x80\x80 Reiniciar\n"
            "  \xEE\x80\x81 Cancelar"
        ),
        STR_NL(
            "Terugkeren naar \xEE\x81\xB3HOME is niet\n"
            "beschikbaar.Wil je de console herstarten?\n\n"
            "  \xEE\x80\x80 Herstarten\n"
            "  \xEE\x80\x81 Annuleren"
        ),
        STR_RU(
            "Возврат к \xEE\x81\xB3HOME недоступен.\n"
            "Вы собираетесь перезагрузить консоль.\n\n"
            "  \xEE\x80\x80 Перезагрузите\n"
            "  \xEE\x80\x81 Отмена"
        ),
        STR_ZH_HANS(
            "无法返回至主机的 \xEE\x81\xB3HOME 菜单。\n"
            "您需要重新启动您的 3DS 设备。\n\n"
            "  \xEE\x80\x80 重启设备\n"
            "  \xEE\x80\x81 取消操作"
        ),
        STR_ZH_HANT(
            "無法返回至主機的 \xEE\x81\xB3HOME 選單。\n"
            "您需要重新啓動您的 3DS 設備。\n\n"
            "  \xEE\x80\x80 重啓設備\n"
            "  \xEE\x80\x81 取消操作"
        ),
    },*/

    /*[StrId_ReturnToHome] =
    {
        STR_EN(
            "You're about to return to \xEE\x81\xB3HOME.\n\n"
            "  \xEE\x80\x80 Return\n"
            "  \xEE\x80\x81 Cancel\n"
            "  \xEE\x80\x82 Reboot"
        ),
        STR_ES(
            "Está a punto de volver a \xEE\x81\xB3HOME.\n\n"
            "  \xEE\x80\x80 Volver\n"
            "  \xEE\x80\x81 Cancelar\n"
            "  \xEE\x80\x82 Reiniciar"
        ),
        STR_DE(
            "Rückkehr zum \xEE\x81\xB3HOME-Menü.\n\n"
            "  \xEE\x80\x80 Fortfahren\n"
            "  \xEE\x80\x81 Abbrechen\n"
            "  \xEE\x80\x82 Konsole neustarten"
        ),
        STR_FR(
            "Retour au menu \xEE\x81\xB3HOME.\n\n"
            "  \xEE\x80\x80 Continuer\n"
            "  \xEE\x80\x81 Annuler\n"
            "  \xEE\x80\x82 Redémarrer"
        ),
        STR_IT(
            "Ritorno al menu \xEE\x81\xB3HOME.\n\n"
            "  \xEE\x80\x80 Continua\n"
            "  \xEE\x80\x81 Annulla\n"
            "  \xEE\x80\x82 Riavvia"
        ),
        STR_JP(
            " \xEE\x81\xB3HOME に戻ろうとしています。\n\n"
            "  \xEE\x80\x80 了解\n"
            "  \xEE\x80\x81 キャンセル\n"
            "  \xEE\x80\x82 再起動"
        ),
        STR_KO(
            "\xEE\x81\xB3HOME으로 돌아가려고 합니다.\n\n"
            "  \xEE\x80\x80 돌아가기\n"
            "  \xEE\x80\x81 취소\n"
            "  \xEE\x80\x82 재부팅"
        ),
        STR_PT(
            "Regressar ao menu \xEE\x81\xB3HOME.\n\n"
            "  \xEE\x80\x80 Regressar\n"
            "  \xEE\x80\x81 Cancelar\n"
            "  \xEE\x80\x82 Reiniciar"
        ),
        STR_NL(
            "Je keert zo terug naar \xEE\x81\xB3HOME.\n\n"
            "  \xEE\x80\x80 Doorgaan\n"
            "  \xEE\x80\x81 Annuleren\n"
            "  \xEE\x80\x82 Herstarten"
        ),
        STR_RU(
            "Вы возвращаетесь в \xEE\x81\xB3HOME.\n\n"
            "  \xEE\x80\x80 Вернуть\n"
            "  \xEE\x80\x81 Отмена\n"
            "  \xEE\x80\x82 Перезагрузите"
        ),
        STR_ZH_HANS(
            "您即将返回到主機的 \xEE\x81\xB3HOME 菜单。\n\n"
            "  \xEE\x80\x80 确认返回\n"
            "  \xEE\x80\x81 取消操作\n"
            "  \xEE\x80\x82 重启设备"
        ),
        STR_ZH_HANT(
            "您即將返回到主機的 \xEE\x81\xB3HOME 選單。\n\n"
            "  \xEE\x80\x80 確認返回\n"
            "  \xEE\x80\x81 取消操作\n"
            "  \xEE\x80\x82 重啓設備"
        ),
    },*/

    /*[StrId_TitleSelector] =
    {
        STR_EN("Title selector"),
        STR_ES("Selector de título"),
        STR_DE("Titel-Auswahl"),
        STR_FR("Sélecteur de titre"),
        STR_IT("Selettore del titolo"),
        STR_JP("タイトルセレクタ"),
        STR_KO("타이틀 선택기"),
        STR_PT("Selector de Títulos"),
        STR_NL("Titel selector"),
        STR_RU("Селектор заголовков"),
        STR_ZH_HANS("应用启动器"),
        STR_ZH_HANT("自製程式啓動器"),
    },

    [StrId_ErrorReadingTitleMetadata] =
    {
        STR_EN("Error reading title metadata.\n%08lX%08lX@%d"),
        STR_ES("Error leyendo los metadatos de los títulos.\n%08lX%08lX@%d"),
        STR_DE("Fehler beim Lesen der Titel-Metadaten.\n%08lX%08lX@%d"),
        STR_FR(
            "Erreur lors de la lecture des métadonnées\n"
            "du titre.\n%08lX%08lX@%d"
        ),
        STR_IT("Errore nella lettura dei metadata dei titoli.\n%08lX%08lX@%d"),
        STR_JP("タイトルメタデータを読み取ることができませんでした。\n%08lX%08lX@%d"),
        STR_KO("타이틀 메타데이터를 읽는데 실패하였습니다.\n%08lX%08lX@%d"),
        STR_PT("Erro a ler os metadados do título.\n%08lX%08lX@%d"),
        STR_NL("Fout bij het lezen van titel metadata.\n%08lX%08lX@%d"),
        STR_RU("Ошибка чтения метаданных заголовка\n.%08lX%08lX@%d"),
        STR_ZH_HANS("读取软件相关信息时发生错误：\n%08lX%08lX@%d"),
        STR_ZH_HANT("讀取軟體相關資訊時發生錯誤：\n%08lX%08lX@%d"),
    },

    [StrId_NoTitlesFound] =
    {
        STR_EN("No titles could be detected."),
        STR_ES("No se han podido detectar títulos."),
        STR_DE("Keine Titel gefunden."),
        STR_FR("Aucun titre trouvé."),
        STR_IT("Nessun titolo trovato."),
        STR_JP("タイトルが見つかりませんでした。"),
        STR_KO("타이틀을 찾을 수 없습니다."),
        STR_PT("Nenhum título foi encontrado."),
        STR_NL("Geen titels gevonden."),
        STR_RU("Заголовки не обнаружены"),
        STR_ZH_HANS("主机内找不到任何软件。"),
        STR_ZH_HANT("主機内找不到任何軟體。"),
    },

    [StrId_SelectTitle] =
    {
        STR_EN(
            "Please select a target title.\n\n"
            "  \xEE\x80\x80 Select\n"
            "  \xEE\x80\x81 Cancel"
        ),
        STR_ES(
            "Elija el título de destino.\n\n"
            "  \xEE\x80\x80 Seleccionar\n"
            "  \xEE\x80\x81 Cancelar"
        ),
        STR_DE(
            "Bitte wähle den Ziel-Titel aus.\n\n"
            "  \xEE\x80\x80 Auswählen\n"
            "  \xEE\x80\x81 Abbrechen"
        ),
        STR_FR(
            "Veuillez sélectionner un titre de destination.\n\n"
            "  \xEE\x80\x80 Sélectionner\n"
            "  \xEE\x80\x81 Annuler"
        ),
        STR_IT(
            "Seleziona il titolo di destinazione.\n\n"
            "  \xEE\x80\x80 Seleziona\n"
            "  \xEE\x80\x81 Annulla"
        ),
        STR_JP(
            "ターゲットタイトルを選択してください。\n\n"
            "  \xEE\x80\x80 選択\n"
            "  \xEE\x80\x81 キャンセル"
        ),
        STR_KO(
            "대상 타이틀을 선택해 주세요.\n\n"
            "  \xEE\x80\x80 선택\n"
            "  \xEE\x80\x81 취소"
        ),
        STR_PT(
            "Por favor escolha um título alvo.\n\n"
            "  \xEE\x80\x80 Escolher\n"
            "  \xEE\x80\x81 Cancelar"
        ),
        STR_NL(
            "Selecteer een titel.\n\n"
            "  \xEE\x80\x80 Selecteer\n"
            "  \xEE\x80\x81 Annuleren"
        ),
        STR_RU(
            "Выберите целевой заголовок.\n\n"
            "  \xEE\x80\x80 Выберите\n"
            "  \xEE\x80\x81 Отмена"
        ),
        STR_ZH_HANS(
            "请选择一个目标软件。\n\n"
            "  \xEE\x80\x80 确认\n"
            "  \xEE\x80\x81 取消"
        ),
        STR_ZH_HANT(
            "請選擇一個目標軟體。\n\n"
            "  \xEE\x80\x80 確認\n"
            "  \xEE\x80\x81 取消"
        ),
    },

    [StrId_NoTargetTitleSupport] =
    {
        STR_EN(
            "This homebrew exploit does not have support\n"
            "for launching applications under target titles.\n"
            "Please use a different exploit."
        ),
        STR_ES(
            "Este exploit de homebrew no tiene soporte para\n"
            "ejecutar aplicaciones bajo títulos de destino.\n"
            "Use otro exploit diferente."
        ),
        STR_DE(
            "Dieser Homebrew-Exploit unterstützt das Starten\n"
            "von Anwendungen unter Ziel-Titeln nicht.\n"
            "Bitte verwende einen anderen Exploit."
        ),
        STR_FR(
            "Cet exploit homebrew ne permet pas de lancer\n"
            "des applications sous des titres précis.\n"
            "Veuillez utiliser un exploit différent."
        ),
        STR_IT(
            "Questo exploit homebrew non permette di avviare\n"
            "applicazioni in titoli specifici.\n"
            "Utilizza un exploit diverso."
        ),
        STR_JP(
            "この自作エクスプロイトでは、ターゲットタイトルの\n"
            "下でアプリを起動することができません。\n"
            "別のエクスプロイトを使用してください。"
        ),
        STR_KO(
            "이 홈브류 익스플로잇은 해당 타이틀에서 애플리케이션을\n"
            "실행하는 것을 지원하지 않습니다.\n"
            "다른 익스플로잇을 사용해 주세요."
        ),
        STR_PT(
            "Este exploit homebrew não têm suporte\n"
            "para executar aplicações no título alvo.\n"
            "Por favor use um exploit diferente."
        ),
        STR_NL(
            "Deze homebrew exploit heeft geen ondersteuning\n"
            "voor het starten van toepassingen met de gekozen titlel.\n"
            "Gebruik een andere exploit."
        ),
        STR_RU(
            "Этот эксплойт homebrew не поддерживает запуск\n"
            "приложений под целевыми заголовками.\n"
            "Пожалуйста, используйте другой эксплойт."
        ),
        STR_ZH_HANS(
            "您所利用漏洞启动的「自制软件启动器」，\n"
            "无法在当前选中的软件中启动自制软件。\n"
            "请使用其它的漏洞来启动「自制软件启动器」。"
        ),
        STR_ZH_HANT(
            "您所利用漏洞開啓的「自製軟體啓動器」\n"
            "無法在當前選中的軟體啓動自製軟體。\n"
            "請利用其它漏洞來啓動「自製軟體啓動器」。"
        ),
    },

    [StrId_MissingTargetTitle] =
    {
        STR_EN(
            "The application you attempted to run requires\n"
            "a title that is not installed in the system."
        ),
        STR_ES(
            "La aplicación seleccionada necesita un título\n"
            "que no está instalado en el sistema."
        ),
        STR_DE(
            "Die ausgewählte Anwendung benötigt einen\n"
            "Titel der nicht installiert ist."
        ),
        STR_FR(
            "L'application sélectionnée requiert un titre\n"
            "qui n'a pas été installé sur le système."
        ),
        STR_IT(
            "L'applicazione selezionata richiede un titolo\n"
            "che non è installato nel sistema."
        ),
        STR_JP(
            "このアプリを実行するために\n"
            "必要なタイトルがインストールされていません。"
        ),
        STR_KO(
            "해당 애플리케이션은 시스템에 설치되지 않은\n"
            "타이틀을 요구합니다."
        ),
        STR_PT(
            "A aplicação que acabou de tentar executar requer\n"
            "um título que não está instalado neste sistema."
        ),
        STR_NL(
            "De toepassing die je probeert te starten\n"
            "vereist een titel die niet geinstalleerd is."
        ),
        STR_RU(
            "Для приложения требуется зависимость,\n"
            "которая не установлена."
        ),
        STR_ZH_HANS(
            "主机找不到该应用程序\n"
            "所需求的软件。"
        ),
        STR_ZH_HANT(
            "主機找不到該應用程式\n"
            "所需求的軟體。"
        ),
    },*/

    [StrId_NetLoader] =
    {
        STR_EN("NetLoader"),
        STR_ES("Cargador de programas"),
        STR_DE("Netzwerk-Loader"),
        STR_FR("NetLoader"),
        STR_IT("Caricamento programmi"),
        STR_JP("ネットローダ"),
        STR_KO("네트워크 로더"),
        STR_PT("Carregador de programas"),
        STR_NL("netwerk lader"),
        STR_RU("NetLoader"),
        STR_ZH_HANS("从网络加载程序"),
        STR_ZH_HANT("從網路載入程式"),
    },

    [StrId_NetLoaderUnavailable] =
    {
        STR_EN("The NetLoader is currently unavailable."),
        STR_ES("El cargador de programas no está disponible."),
        STR_DE("Der Netzwerk-Loader ist zur Zeit nicht verfügbar."),
        STR_FR("Le programme nxlink est indisponible."),
        STR_IT("Il caricamento programmi nxlink non è disponibile."),
        STR_JP("nxlinkネットローダは現在利用できません。"),
        STR_KO("현재 네트워크 로더는 사용이 불가합니다."),
        STR_PT("O carregador de programas está de momento indisponível."),
        STR_NL("De netwerk lader is niet beschikbaar."),
        STR_RU("NetLoader в настоящее время недоступен."),
        STR_ZH_HANS("无法启动 nxlink 网络执行模块。"),
        STR_ZH_HANT("無法啓動 nxlink 網路執行模組。"),
    },

    [StrId_NetLoaderError] =
    {
        STR_EN("An error occurred.\nTechnical details: [%s:%d]"),
        STR_ES("Ha ocurrido un error.\nDatos técnicos: [%s:%d]"),
        STR_DE("Ein Fehler ist aufgetreten\nTechnische Details: [%s:%d]"),
        STR_FR("Une erreur s'est produite.\nDétails techniques : [%s:%d]"),
        STR_IT("Si è verificato un errore.\nDettagli tecnici : [%s:%d]"),
        STR_JP("エラーが発生しました。\n技術的な詳細：[%s:%d]"),
        STR_KO("오류가 발생했습니다.\n기술적인 세부사항: [%s:%d]"),
        STR_PT("Ocorreu um erro.\nDetalhes técnicos: [%s:%d]"),
        STR_NL("Er is een fout opgetreden\nTechnische details: [%s:%d]"),
        STR_RU("Произошла ошибка.\nТехнические подробности: [%s:%d]"),
        STR_ZH_HANS("发生错误。\n详细错误信息：[%s:%d]"),
        STR_ZH_HANT("發生錯誤。\n詳細錯誤資訊：[%s:%d]"),
    },

    [StrId_NetLoaderOffline] =
    {
        STR_EN("Offline, waiting for network…"),
        STR_DE("Offline, warte auf Netzwerk…"),
        STR_FR("Hors-ligne, en attente d'une connection..."),
        STR_IT("Disconnesso, in attesa della connessione…"),
        STR_ES("Desconectado, esperando a la red..."),
        STR_JP("オフラインです。ネットワーク接続を待っています…"),
        STR_KO("연결 끊김, 네트워크 기다리는 중…"),
        STR_ZH_HANS("无法连接网络，等待网络连接…"),
        STR_ZH_HANT("目前已離線，等待網路連線…"),
        STR_RU("Оффлайн, ожидание сети…"),
    },

    [StrId_NetLoaderActive] =
    {
        STR_EN(
            "Waiting for nxlink to connect…\n"
            "IP Addr: %lu.%lu.%lu.%lu, Port: %d"
        ),
        STR_ES(
            "Esperando a que se conecte nxlink…\n"
            "Dir.IP: %lu.%lu.%lu.%lu, Puerto: %d"
        ),
        STR_DE(
            "Warte auf Verbindung von nxlink…\n"
            "IP Addr: %lu.%lu.%lu.%lu, Port: %d"
        ),
        STR_FR(
            "En attente de la connexion de nxlink…\n"
            "Adr. IP : %lu.%lu.%lu.%lu, Port : %d"
        ),
        STR_IT(
            "In attesa della connessione di nxlink…\n"
            "Ind. IP : %lu.%lu.%lu.%lu, Porta : %d"
        ),
        STR_JP(
            "nxlinkが接続されるのを待っています…\n"
            "IPアドレス：%lu.%lu.%lu.%lu, ポート番号：%d"
        ),
        STR_KO(
            "nxlink의 연결을 대기중…\n"
            "IP 주소: %lu.%lu.%lu.%lu, 포트: %d"
        ),
        STR_PT(
            "A aguardar pela conexão do nxlink…\n"
            "End. IP: %lu.%lu.%lu.%lu, Porta: %d"
        ),
        STR_NL(
            "Wachten op nxlink verbinding…\n"
            "IP Addr: %lu.%lu.%lu.%lu, Poort: %d"
        ),
        STR_RU(
            "Ожидание подключения nxlink…\n"
            "IP-адрес: %lu.%lu.%lu.%lu, Порт: %d"
        ),
        STR_ZH_HANS(
            "等待 nxlink 连接…\n"
            "IP 地址：%lu.%lu.%lu.%lu，端口：%d"
        ),
        STR_ZH_HANT(
            "等待 nxlink 連接…\n"
            "IP 位址：%lu.%lu.%lu.%lu，連接埠：%d"
        ),
    },

    [StrId_NetLoaderTransferring] =
    {
        STR_EN(
            "Transferring…\n"
            "%zu out of %zu KiB written"
        ),
        STR_ES(
            "Transfiriendo…\n"
            "%zu de %zu KiB escritos"
        ),
        STR_DE(
            "Übertragen…\n"
            "%zu von %zu KiB geschrieben"
        ),
        STR_FR(
            "Transfert…\n"
            "%zu sur %zu Kio écrits"
        ),
        STR_IT(
            "Trasferimento…\n"
            "%zu di %zu KiB scritti"
        ),
        STR_JP(
            "データを転送しています…\n"
            "%zu / %zu KiB 転送済み"
        ),
        STR_KO(
            "전송 중…\n"
            "%zu / %zu KiB 쓰여짐"
        ),
        STR_PT(
            "A transferir…\n"
            "%zu de %zu KiB escritos"
        ),
        STR_NL(
            "Overbrengen…\n"
            "%zu van %zu KiB geschreven"
        ),
        STR_RU(
            "Передача…\n"
            "%zu из %zu КиБ получено"
        ),
        STR_ZH_HANS(
            "正在传输…\n"
            "已完成 %zu / %zu KiB"
        ),
        STR_ZH_HANT(
            "正在傳輸…\n"
            "已完成 %zu / %zu KiB"
        ),
    },
};

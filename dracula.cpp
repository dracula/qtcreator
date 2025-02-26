#include <coreplugin/coreconstants.h>
#include <coreplugin/icontext.h>
#include <coreplugin/icore.h>
#include <extensionsystem/iplugin.h>
#include <utils/filepath.h>
#include <utils/fileutils.h>

using namespace Core;
using namespace Utils;

namespace Dracula::Internal {

class DraculaPlugin final : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "Dracula.json")

public:
    void initialize() final
    {
        FilePath draculaXml = FilePath::fromString(":/dracula.xml");
        FilePath drakulaTheme = FilePath::fromString(":/drakula.creatortheme");
        FilePath drakulaTokens = FilePath::fromString(":/drakula.figmatokens");

        FilePath stylesDir = ICore::userResourcePath("styles");
        FilePath themesDir = ICore::userResourcePath("themes");

        struct FileToDir
        {
            FilePath file;
            FilePath dir;
        };
        QVector<FileToDir> fileToDir{{draculaXml, stylesDir},
                                     {drakulaTheme, themesDir},
                                     {drakulaTokens, themesDir}};

        // "Install" the theme and text color files
        for (const auto &pair : fileToDir) {
            if (!pair.dir.exists())
                pair.dir.createDir();
            auto destFile = pair.dir.pathAppended(pair.file.fileName());
            FileUtils::copyIfDifferent(pair.file, destFile);
        }

        auto settings = ICore::settings();
        const QString themeName = drakulaTheme.baseName();
        if (settings->value(Constants::SETTINGS_THEME) != themeName) {
            // Set the theme and text color scheme as default
            settings->setValue(Constants::SETTINGS_THEME, themeName);

            const char schemeFileNamesKey[] = "TextEditor/ColorSchemes";
            auto schemeFileNames = settings->value(schemeFileNamesKey).toMap();
            schemeFileNames.insert(themeName, draculaXml.toSettings());
            settings->setValue(schemeFileNamesKey, schemeFileNames);

            ICore::askForRestart(tr("The Drakula theme change will take effect after restart."));
        }
    }
};

} // namespace Dracula::Internal

#include <dracula.moc>

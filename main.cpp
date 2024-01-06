// FIXME: Добавить поддержку смайлов, сделать логирование разноцветным(?)

// C++ Библиотеки
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <codecvt>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <random>
#include <thread>
#include <mutex>
#include <filesystem>

// C Библиотеки

using namespace std;

enum class LogLevel
{
    TRACE,
    DEBUG,
    INFO,
    WARNING,
    ERROR
};

class Logger
{
    public:
        Logger()
        {
            // Создание уникального файла логов
            LogOutput = make_unique<ostream>(cout.rdbuf());
        }

        // Функция для задания места хранения логов
        void SetLogDir(const string& directory)
        {
            LogDir = directory;
            // Создание директории, если её нет
            filesystem::create_directories(directory);
        }

        // Функция для настройки вывода логов (в файл и/или в папку)
        void SetLogDestination(const string& destination)
        {

        }

        // Функция логирования 
        void log(LogLevel level, const wstring& message)
        {
            // Синхронное логирование
            lock_guard<mutex> lock(LogMutex);

            // Переключение уровней логирования
            string LevelString;
            switch (level)
            {
                case LogLevel::TRACE:
                    LevelString = "TRACE";
                    break;

                case LogLevel::DEBUG:
                    LevelString = "DEBUG";
                    break;

                case LogLevel::INFO:
                    LevelString = "INFO";
                    break;

                case LogLevel::WARNING:
                    LevelString = "WARNING";
                    break;

                case LogLevel::ERROR:
                    LevelString = "ERROR";
                    break;
            }

            // Измерение времени для вывода в лог
            auto now = chrono::system_clock::to_time_t(chrono::system_clock::now());
            tm tm = *localtime(&now);

            // Задаём случайное значение для имени файла
            random_device rd;
            mt19937 gen(rd());
            uniform_int_distribution<> dis(1,1000);
            int RandomNumber = dis(gen);

            // Задаём имени файла лога
            stringstream FilenameGenerator;
            FilenameGenerator << LogDir << "log_" << put_time(&tm, "%d-%m-%Y_%H-%M-%S") << "_" << RandomNumber << ".log";
            string LogFilename = FilenameGenerator.str();

            // Создаём уникальный указатель на файл
            unique_ptr<ofstream> LogFile = make_unique<ofstream>(LogFilename, ios::out | ios::app);

            // Включаем поддержку Unicode (UTF-8) !!!! Пока работает некорректно !!!!
            wstring_convert<codecvt_utf8<wchar_t>> converter;
            string utf8_message = converter.to_bytes(message);

            // Задаём формат строки в логе
            (*LogFile) << "[" << put_time(&tm, "%d-%m-%Y %H:%M:%S") << "][" << LevelString << "] " << utf8_message << endl;
            (*LogOutput) << "Logged to file: " << LogFilename << endl;
        }

        // Чистка папки с логами
        void ClearLogDir(const string& directory)
        {
            for (const auto& files : filesystem::directory_iterator(directory))
                filesystem::remove_all(files.path());
        }

    private:
        unique_ptr<ostream> LogOutput;
        string LogDir;
        mutex LogMutex;
};


// Driver Program 
int main()
{
    
    // Default Settings
    // Создание объекта класса
    Logger logger;
    // Задаём место хранения логов
    logger.SetLogDir("C:/Logs/");

    // User Interface //
    string answer = "";
    cout << "\nЭта программа собирает логи с другой программы\n";

    // Задаём директорию для вывода логов (по умолчанию: "C:/Logs/")
    cout << "\nВы хотите сменить папку для вывода логов? (по умолчанию: 'C:/Logs/)\n\n \t\tYes\t\t No\t\t\t\n:";
    cin >> answer;
    if (answer == "Yes" or answer == "yes" or answer == "y" or answer == "Y")
        {
            cout << "\nОК. Выберите место хранения (К примеру, 'D:/test/<log_dir>/'): ";
            cin >> answer;
            logger.SetLogDir(answer);
            cout << "\nВаши логи находятся здесь -" << answer << endl << endl; 
        }
        else 
        {
            logger.SetLogDir("C:/logs/");
            cout << "\nВаши логи находятся здесь - 'C:/logs/\n";
        }

    // Задаём уровень логирования !!! ДОБАВИТЬ ПОДДЕРЖКУ СМАЙЛОВ  🖂!!!
    logger.log(LogLevel::TRACE, L"Поддержка Unicode сообщения: Привет, TRACE! ");
    logger.log(LogLevel::DEBUG, L"Поддержка Unicode сообщения: Привет, DEBUG! ");
    logger.log(LogLevel::INFO, L"Поддержка Unicode сообщения: Привет, INFO! ");
    logger.log(LogLevel::WARNING, L"Поддержка Unicode сообщения: Привет, WARNING! ");
    logger.log(LogLevel::ERROR, L"Поддержка Unicode сообщения: Привет, ERROR! ");

    // Чистка логов
    cout << "\nВы хотите удалить логи?\n\n \t\tYes\t\t No\t\t\t\n:";
    answer = "";
    cin >> answer;
    if (answer == "Yes" or answer == "Y" or answer == "y" or answer == "Y")
        {
            logger.ClearLogDir("C:/logs/");
            cout << "\nЛоги были удалены";
        }
        else cout << "OK!\n";
    
    return 0;
}
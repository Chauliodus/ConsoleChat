#include <QCoreApplication>
#include <iostream>
#include <fstream>
#include <mutex>
#include <thread>
#include <chrono>

std::mutex mtx;

void chat(const std::string& path, size_t id, const std::string& message, const size_t& delay)
{
    while(1)
    {
        std::this_thread::sleep_for(std::chrono::seconds(delay));

        {
            std::lock_guard<std::mutex> lG(mtx);

            std::fstream file(path, std::ios_base::app);
            if (!file.is_open())
            {
                std::cerr << "cannot open file " << path;
                exit(0);
            }
            char buffer[80];
            time_t seconds = time(NULL);
            tm *timeinfo = localtime(&seconds);
            char* format = "%I:%M:%S";
            strftime(buffer, 80, format, timeinfo);
            file << "[" << buffer << "]; " << id << ": " << message << std::endl;
            std::cout << "[" << buffer << "]; " << id << ": " << message << std::endl;
            file.close();
        }
    }
}

void inputFcn(std::string& path, size_t& number)
{
    std::cout << "Input path: ";
    std::cin >> path;
    std::cout << "Input number of clients: ";
    std::cin >> number;
}

void inputFcn2(size_t id, std::string& message, size_t& delay)
{
    std::cout << id + 1 << " delay: ";
    std::cin >> delay;
    std::cout << id + 1 << " message: ";
    std::cin >> message;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    std::string path;
    size_t numberOfClients;
    inputFcn(path, numberOfClients);

    std::vector<std::string> vectorMessage;
    std::vector<size_t> vectorDelay;
    std::vector<std::thread> vectorThread;
    std::string message;
    size_t delay;

    for(size_t id = 0; id < numberOfClients; id++)
    {
        inputFcn2(id, message, delay);
        vectorMessage.push_back(message);
        vectorDelay.push_back(delay);
    }

    for(size_t id = 0; id < numberOfClients; id++)
    {
        vectorThread.push_back(std::thread(chat, std::ref(path),
                                           id + 1,
                                           vectorMessage[id],
                                           vectorDelay[id]));
        vectorThread[id].detach();
    }

    return a.exec();
}

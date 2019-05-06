#include <QCoreApplication>
#include <iostream>
#include <fstream>
#include <mutex>
#include <thread>
#include <chrono>

std::mutex mtx;

void chat(const std::string& path, int index, const std::string& message, const int delay)
{
    while(1)
    {
        mtx.lock();

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
        file << "[" << buffer << "]; " << index << ": " << message << std::endl;
        std::cout << "[" << buffer << "]; " << index << ": " << message << std::endl;
        file.close();

        mtx.unlock();

        std::this_thread::sleep_for(std::chrono::seconds(delay));
    }
}

void inputFcn(std::string& path, int& number)
{
    std::cout << "Input path: ";
    std::cin >> path;
    std::cout << "Input number of clients: ";
    std::cin >> number;
}

void inputFcn2(int index, std::string& message, int& delay)
{
    std::cout << index + 1 << " message: ";
    std::cin >> message;
    std::cout << index + 1 << " delay: ";
    std::cin >> delay;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    std::string path;
    int numberOfClients;
    inputFcn(path, numberOfClients);

    std::vector<std::string> messages;
    std::vector<int> delays;
    std::vector<std::thread> threads;
    std::string message;
    int delay;

    for(int i = 0; i < numberOfClients; i++)
    {
        inputFcn2(i, message, delay);
        messages.push_back(message);
        delays.push_back(delay);
    }

    for(int i = 0; i < numberOfClients; i++)
    {
        threads.push_back(std::thread(chat, path, i + 1, messages[i], delays[i]));
        //threads[i].join();
    }

    messages.clear();
    delays.clear();

    return a.exec();
}

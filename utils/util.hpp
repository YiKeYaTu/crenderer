//
// Created by lc on 2022/2/24.
//

#ifndef CRENDERER_UTIL_HPP
#define CRENDERER_UTIL_HPP

#include <vector>
#include <cmath>
#include <string>
#include <random>
#include <queue>
#include <thread>
#include <map>

namespace util {
constexpr double MY_PI = 3.14159265358979323846;
double getRandom01() {
    static std::random_device dev;
    static std::mt19937 rng(dev());
    static std::uniform_real_distribution<float> dist(0.f, 1.f);
    return dist(rng);
}
std::vector<std::string> split(std::string& s, const std::string& separator) {
    std::vector<std::string> tokens;

    for (int i = 0, j; ; i = j + separator.length()) {
        j = s.find(separator, i);
        if (j == -1) {
            tokens.push_back(s.substr(i, s.size() - i));
            break;
        } else {
            tokens.push_back(s.substr(i, j - i));
        }
    }

    return tokens;
}

std::string trim(std::string& s) {
    int i = 0, j = s.size() - 1;

    while (i < s.size()) {
        if (s[i] != ' ') break;
        i ++;
    }
    while (j > i) {
        if (s[j] != ' ') break;
        j --;
    }

    return s.substr(i, j - i + 1);
}

double clamp(double min, double max, double val) {
    return std::fmax(min, std::fmin(max, val));
}

void saveFrameBufferAsPPM(const char* fileName, std::vector<Vec3f>& frameBuffer, int width, int height) {
    FILE* fp = fopen(fileName, "wb");
    (void)fprintf(fp, "P6\n%d %d\n255\n", width, height);
    for (auto i = 0; i < height * width; ++i) {
        static unsigned char color[3];
        color[0] = (unsigned char)(255 * std::pow(util::clamp(0, 1, frameBuffer[i].x()), 0.6f));
        color[1] = (unsigned char)(255 * std::pow(util::clamp(0, 1, frameBuffer[i].y()), 0.6f));
        color[2] = (unsigned char)(255 * std::pow(util::clamp(0, 1, frameBuffer[i].z()), 0.6f));
        fwrite(color, 1, 3, fp);
    }
    fclose(fp);
}

void UpdateProgress(float progress)
{
    static float prevProgress = 0.0;
    int barWidth = 70;

    if (prevProgress > progress) {
        return;
    }

    prevProgress = progress;

    std::cout << "[";
    int pos = barWidth * progress;
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << int(progress * 100.0) << " %\r";
    std::cout.flush();
};

class ThreadPool {
private:
    std::queue<const std::function<void()>> waitList_;
    std::map<int, std::thread> threads;

    std::size_t limit_;
    std::mutex m_;
    std::condition_variable condEmpty_;
    std::condition_variable condFull_;
    bool stop_ = false;
public:
    ThreadPool(std::size_t limit, std::size_t numThread) {
        for (int i = 0; i < numThread; ++i) {
            threads.insert({ i, std::thread([&]{
                while (true) {
                    std::unique_lock uniqueLock(m_);
                    bool f = condFull_.wait_for(uniqueLock, std::chrono::seconds(1), [&]{ return waitList_.size() > 0; });

                    if (f && waitList_.size() > 0) {
                        auto front = waitList_.front();
                        waitList_.pop();
                        uniqueLock.unlock();
//                        condEmpty_.notify_one();

                        try {
                            front();
                        } catch (std::exception& e) {
                            std::cout << e.what() << std::endl;
                            return;
                        }
                    } else if (stop_) {
                        std::cout << "stop!!" << std::endl;
                        return;
                    }
                }
            })});
        }
    }

    void allJoin() {
        for (auto& thread : threads) {
            thread.second.join();
        }
    }

    void stop() {
        stop_ = true;
    }

    void tryAppendTask(const std::function<void()>& task) {
        std::unique_lock lockGuard(m_);
//        condEmpty_.wait(lockGuard, [&]{ return waitList_.size() < limit_; });
        waitList_.push(task);
        condFull_.notify_one();
    }
};
}

#endif //CRENDERER_UTIL_HPP

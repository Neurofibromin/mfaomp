#ifndef CURRENTBACKENDSTATUSSINGLETON_H
#define CURRENTBACKENDSTATUSSINGLETON_H

enum currentBackEnd {
    QMediaPlayerBackEnd,
    VLCPlayerBackEnd,
    QWebEngineBackEnd
};

// Probably not best practice
// Singleton class to store the current backend status
class CurrentBackEndStatusSingleton {
public:
    static CurrentBackEndStatusSingleton& getInstance() {
        static CurrentBackEndStatusSingleton instance;
        return instance;
    }
    void setCurrentBackEnd(currentBackEnd backend) {
        currentBackend_ = backend;
    }
    currentBackEnd getCurrentBackEnd() const {
        return currentBackend_;
    }
private:
    CurrentBackEndStatusSingleton() : currentBackend_(VLCPlayerBackEnd) {}
    CurrentBackEndStatusSingleton(const CurrentBackEndStatusSingleton&) = delete;
    CurrentBackEndStatusSingleton& operator=(const CurrentBackEndStatusSingleton&) = delete;
    currentBackEnd currentBackend_;
};

#endif //CURRENTBACKENDSTATUSSINGLETON_H

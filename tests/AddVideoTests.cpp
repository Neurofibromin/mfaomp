//AddVideoTests.cpp

#define CATCH_CONFIG_EXTERNAL_INTERFACES
#include <catch2/reporters/catch_reporter_event_listener.hpp>
#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/reporters/catch_reporter_registrars.hpp>
#include "AddVideo.h"
#include "MainWindow.h"
#include "MediaPlayers.h"
#include "PlayerFactory.h"
#include <QApplication>
#include <QCoreApplication>
#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>
#include <QGridLayout>
#include <QObject>
#include <QSignalSpy>
#include <QTest>
#include <QUrl>
#include <QVector>
#include <QWidget>
#include <iostream>

static int g_test_argc = 1;
static char app_name_str[] = "mfaomp_tests_executable";
static char* g_test_argv[] = {app_name_str, nullptr};

namespace {
    class QtAppManagerListener : public Catch::EventListenerBase {
    public:
        using Catch::EventListenerBase::EventListenerBase;

        QApplication* currentApp = nullptr;

        void testRunStarting(Catch::TestRunInfo const& /*testRunInfo*/) override {
            if (!QApplication::instance()) {
                currentApp = new QApplication(g_test_argc, g_test_argv);
            }
        }

        void testRunEnded(Catch::TestRunStats const& /*testRunStats*/) override {
            if (currentApp) {
                delete currentApp;
                currentApp = nullptr;
            }
        }
    };
}

CATCH_REGISTER_LISTENER(QtAppManagerListener)

class MockMediaPlayer : public MediaPlayerBase {
public:
    explicit MockMediaPlayer(const QUrl& url) : MediaPlayerBase(url) {
        videoWidget = new QWidget();

        videoWidget->setObjectName("MockVideoWidget_" + url.fileName());
        qDebug() << "Created MockMediaPlayer for URL:" << url.toString();
    }

    ~MockMediaPlayer() override {
        qDebug() << "Destroyed MockMediaPlayer for URL:" << videoUrl.toString();

        delete videoWidget;
    }


    void play() override {
    }

    void pause() override {
    }

    void mute() override {
    }

    void unmute() override {
    }

    void clear() override {
    }

    float speed(float sp = 0.0) override { return 1.0f; }
    int64_t time() override { return 0; }
    void set_time(int64_t time) override {
    }

    float duration() override { return 0.0f; }


    QWidget* getVideoWidget() override {
        return videoWidget;
    }

private:
    QWidget* videoWidget = nullptr;
};


static MediaPlayerBase* (*originalPlayerFactoryCreate)(const QUrl&);


MediaPlayerBase* createMockPlayer(const QUrl& url) {
    return new MockMediaPlayer(url);
}

static QList<QUrl> (*originalQFileDialogGetOpenFileUrls)(QWidget* parent, const QString& caption, const QUrl& dir,
                                                         const QString& filter, QString* selectedFilter,
                                                         QFileDialog::Options options);

static QList<QUrl> mockSelectedUrls;


QList<QUrl> replacementGetOpenFileUrls(QWidget* parent = nullptr,
                                       const QString& caption = QString(),
                                       const QUrl& dir = QUrl(),
                                       const QString& filter = QString(),
                                       QString* selectedFilter = nullptr,
                                       QFileDialog::Options options = QFileDialog::Options(),
                                       const QStringList& supportedSchemes = QStringList()) { //TODO: set default supportedschemes
    Q_UNUSED(parent);
    Q_UNUSED(caption);
    Q_UNUSED(dir);
    Q_UNUSED(filter);
    Q_UNUSED(selectedFilter);
    Q_UNUSED(options);
    qDebug() << "Mock QFileDialog::getOpenFileUrls called, returning:" << mockSelectedUrls;
    return mockSelectedUrls;
}


struct StaticFunctionReplacer {
    void** target;
    void* original;

    StaticFunctionReplacer(void** targetFuncPtr, void* replacementFuncPtr)
        : target(targetFuncPtr), original(*targetFuncPtr) {
        *target = replacementFuncPtr;
    }

    ~StaticFunctionReplacer() {
        *target = original;
    }
};


TEST_CASE("openAndAddVideo - No video files selected", "[AddVideo]") {
    QWidget parentWidget;
    QGridLayout layout;
    QVector<MediaPlayerBase *> mediaPlayers;


    mockSelectedUrls = {};

    SECTION("Initial state") {
        REQUIRE(mediaPlayers.empty());
        REQUIRE(layout.count() == 0);
    }

    openAndAddVideo(parentWidget, layout, mediaPlayers,
                    &createMockPlayer,
                    &replacementGetOpenFileUrls);

    SECTION("After calling") {
        REQUIRE(mediaPlayers.empty());
        REQUIRE(layout.count() == 0);
    }

    for (MediaPlayerBase* player: mediaPlayers) {
        delete player;
    }
    mediaPlayers.clear();
}

TEST_CASE("openAndAddVideo - Single valid video file selected", "[AddVideo]") {
    QWidget parentWidget;
    QGridLayout layout;
    QVector<MediaPlayerBase *> mediaPlayers;

    QString currentSourceFilePath = __FILE__;
    QFileInfo sourceFileInfo(currentSourceFilePath);
    QString currentDirectory = sourceFileInfo.absolutePath();
    QUrl testUrl = QUrl::fromLocalFile(currentDirectory + "/test-data-files/test.mp4");
    std::cout << "Test url: " << testUrl.toString().toStdString();
    QFileInfo testFile(testUrl.toLocalFile());
    REQUIRE(testFile.exists());
    REQUIRE(testFile.isFile());
    REQUIRE(testFile.fileName() == "test.mp4");
    mockSelectedUrls = {testUrl};
    // StaticFunctionReplacer replaceFileDialog(
    //     reinterpret_cast<void **>(&QFileDialog::getOpenFileUrls),
    //     reinterpret_cast<void *>(&replacementGetOpenFileUrls)
    // );
    //
    //
    // StaticFunctionReplacer replacePlayerFactory(
    //     reinterpret_cast<void **>(&PlayerFactory::CreatorInstance), // NOW VALID: Address of the function pointer variable
    //     reinterpret_cast<void *>(&createMockPlayer)
    // );

    SECTION("Initial state") {
        REQUIRE(mediaPlayers.empty());
        REQUIRE(layout.count() == 0);
    }
    openAndAddVideo(parentWidget, layout, mediaPlayers,
                    &createMockPlayer,&replacementGetOpenFileUrls);
    SECTION("After calling") {
        REQUIRE(mediaPlayers.size() == 1);
        REQUIRE(mediaPlayers.first()->videoUrl == testUrl);
        REQUIRE(layout.count() == 1);
        QLayoutItem* item = layout.itemAtPosition(0, 0);
        REQUIRE(item != nullptr);
        REQUIRE(item->widget() != nullptr);
        REQUIRE(item->widget() == mediaPlayers.first()->getVideoWidget());
    }


    for (MediaPlayerBase* player: mediaPlayers) {
        delete player;
    }
    mediaPlayers.clear();
}

TEST_CASE("openAndAddVideo - Multiple valid video files selected", "[AddVideo]") {
    QWidget parentWidget;
    QGridLayout layout;
    QVector<MediaPlayerBase *> mediaPlayers;

    QString currentSourceFilePath = __FILE__;
    QFileInfo sourceFileInfo(currentSourceFilePath);
    QString currentDirectory = sourceFileInfo.absolutePath();
    QUrl testUrl1 = QUrl::fromLocalFile(currentDirectory + "/test-data-files/test.mp4");
    std::cout << "Test url: " << testUrl1.toString().toStdString();
    QFileInfo testFile(testUrl1.toLocalFile());
    REQUIRE(testFile.exists());
    REQUIRE(testFile.isFile());
    REQUIRE(testFile.fileName() == "test.mp4");
    QUrl testUrl2 = QUrl::fromLocalFile(currentDirectory + "/test-data-files/test2.avi");
    std::cout << "Test url: " << testUrl2.toString().toStdString();
    QFileInfo testFile2(testUrl2.toLocalFile());
    REQUIRE(testFile2.exists());
    REQUIRE(testFile2.isFile());
    REQUIRE(testFile2.fileName() == "test2.avi");

    mockSelectedUrls = {testUrl1, testUrl2};
    // StaticFunctionReplacer replaceFileDialog(
    //     reinterpret_cast<void **>(&QFileDialog::getOpenFileUrls),
    //     reinterpret_cast<void *>(&replacementGetOpenFileUrls)
    // );
    //
    // StaticFunctionReplacer replacePlayerFactory(
    //     reinterpret_cast<void **>(&PlayerFactory::CreatorInstance), // NOW VALID: Address of the function pointer variable
    //     reinterpret_cast<void *>(&createMockPlayer)
    // );

    SECTION("Initial state") {
        REQUIRE(mediaPlayers.empty());
        REQUIRE(layout.count() == 0);
    }


    openAndAddVideo(parentWidget, layout, mediaPlayers,
                    &createMockPlayer, &replacementGetOpenFileUrls);

    SECTION("After calling") {
        REQUIRE(mediaPlayers.size() == 2);

        QVector<QUrl> addedUrls;
        for (const auto& player: mediaPlayers) {
            addedUrls.append(player->videoUrl);
        }
        REQUIRE(addedUrls.contains(testUrl1));
        REQUIRE(addedUrls.contains(testUrl2));


        REQUIRE(layout.count() == 2);


        QLayoutItem* item0 = layout.itemAtPosition(0, 0);
        QLayoutItem* item1 = layout.itemAtPosition(0, 1);

        REQUIRE(item0 != nullptr);
        REQUIRE(item0->widget() != nullptr);
        REQUIRE(item1 != nullptr);
        REQUIRE(item1->widget() != nullptr);


        bool widget0FoundInPlayers = false;
        bool widget1FoundInPlayers = false;
        for (const auto& player: mediaPlayers) {
            if (player->getVideoWidget() == item0->widget()) widget0FoundInPlayers = true;
            if (player->getVideoWidget() == item1->widget()) widget1FoundInPlayers = true;
        }
        REQUIRE(widget0FoundInPlayers);
        REQUIRE(widget1FoundInPlayers);


        REQUIRE(layout.itemAtPosition(1, 0) == nullptr);
        REQUIRE(layout.itemAtPosition(1, 1) == nullptr);
    }


    for (MediaPlayerBase* player: mediaPlayers) {
        delete player;
    }
    mediaPlayers.clear();
}

TEST_CASE("addVideoPlayer - Adds player and rearranges layout", "[AddVideo]") {
    QWidget parentWidget;
    QGridLayout layout;
    QVector<MediaPlayerBase *> mediaPlayers;
    QString currentSourceFilePath = __FILE__;
    QFileInfo sourceFileInfo(currentSourceFilePath);
    QString currentDirectory = sourceFileInfo.absolutePath();
    QUrl testUrl1 = QUrl::fromLocalFile(currentDirectory + "/test-data-files/test.mp4");
    std::cout << "Test url: " << testUrl1.toString().toStdString();
    QFileInfo testFile(testUrl1.toLocalFile());
    REQUIRE(testFile.exists());
    REQUIRE(testFile.isFile());
    REQUIRE(testFile.fileName() == "test.mp4");
    QUrl testUrl2 = QUrl::fromLocalFile(currentDirectory + "/test-data-files/test2.avi");
    std::cout << "Test url: " << testUrl2.toString().toStdString();
    QFileInfo testFile2(testUrl2.toLocalFile());
    REQUIRE(testFile2.exists());
    REQUIRE(testFile2.isFile());
    REQUIRE(testFile2.fileName() == "test2.avi");
    mockSelectedUrls = {testUrl1};

    // StaticFunctionReplacer replacePlayerFactory(
    //     reinterpret_cast<void **>(&PlayerFactory::CreatorInstance), // NOW VALID: Address of the function pointer variable
    //     reinterpret_cast<void *>(&createMockPlayer)
    // );

    SECTION("Initial state") {
        REQUIRE(mediaPlayers.empty());
        REQUIRE(layout.count() == 0);
    }


    openAndAddVideo(parentWidget, layout, mediaPlayers,
                    &createMockPlayer, &replacementGetOpenFileUrls);

    SECTION("After adding one player") {
        REQUIRE(mediaPlayers.size() == 1);
        REQUIRE(mediaPlayers.first()->videoUrl == testUrl1);


        REQUIRE(layout.count() == 1);
        QLayoutItem* item = layout.itemAtPosition(0, 0);
        REQUIRE(item != nullptr);
        REQUIRE(item->widget() != nullptr);
        REQUIRE(item->widget() == mediaPlayers.first()->getVideoWidget());
    }

    mockSelectedUrls = {testUrl2};

    openAndAddVideo(parentWidget, layout, mediaPlayers,
                    &createMockPlayer, &replacementGetOpenFileUrls);

    SECTION("After adding a second player") {
        REQUIRE(mediaPlayers.size() == 2);
        REQUIRE(layout.count() == 2);


        QLayoutItem* item0 = layout.itemAtPosition(0, 0);
        QLayoutItem* item1 = layout.itemAtPosition(0, 1);

        REQUIRE(item0 != nullptr);
        REQUIRE(item0->widget() != nullptr);
        REQUIRE(item1 != nullptr);
        REQUIRE(item1->widget() != nullptr);


        bool widget0FoundInPlayers = false;
        bool widget1FoundInPlayers = false;
        for (const auto& player: mediaPlayers) {
            if (player->getVideoWidget() == item0->widget()) widget0FoundInPlayers = true;
            if (player->getVideoWidget() == item1->widget()) widget1FoundInPlayers = true;
        }
        REQUIRE(widget0FoundInPlayers);
        REQUIRE(widget1FoundInPlayers);


        REQUIRE(layout.itemAtPosition(1, 0) == nullptr);
        REQUIRE(layout.itemAtPosition(1, 1) == nullptr);
    }


    for (MediaPlayerBase* player: mediaPlayers) {
        delete player;
    }
    mediaPlayers.clear();
}


TEST_CASE("rearrangeVideoPlayers - Layout logic", "[AddVideo][Layout]") {
    QGridLayout layout;
    QVector<MediaPlayerBase *> mediaPlayers;


    SECTION("0 players") {
        rearrangeVideoPlayers(layout, mediaPlayers);
        REQUIRE(layout.count() == 0);
    }

    SECTION("1 player") {
        mediaPlayers.append(new MockMediaPlayer(QUrl("file://1")));

        rearrangeVideoPlayers(layout, mediaPlayers);

        REQUIRE(layout.count() == 1);

        REQUIRE(layout.itemAtPosition(0, 0) != nullptr);
        REQUIRE(layout.itemAtPosition(0, 0)->widget() == mediaPlayers[0]->getVideoWidget());
        REQUIRE(layout.itemAtPosition(0, 1) == nullptr); // Check bounds
        REQUIRE(layout.itemAtPosition(1, 0) == nullptr);
    }

    SECTION("2 players") {
        mediaPlayers.append(new MockMediaPlayer(QUrl("file://1")));
        mediaPlayers.append(new MockMediaPlayer(QUrl("file://2")));

        rearrangeVideoPlayers(layout, mediaPlayers);

        REQUIRE(layout.count() == 2);

        REQUIRE(layout.itemAtPosition(0, 0) != nullptr);
        REQUIRE(layout.itemAtPosition(0, 0)->widget() == mediaPlayers[0]->getVideoWidget());
        REQUIRE(layout.itemAtPosition(0, 1) != nullptr);
        REQUIRE(layout.itemAtPosition(0, 1)->widget() == mediaPlayers[1]->getVideoWidget());
        REQUIRE(layout.itemAtPosition(1, 0) == nullptr); // Check bounds
        REQUIRE(layout.itemAtPosition(1, 1) == nullptr);
    }

    SECTION("3 players") {
        mediaPlayers.append(new MockMediaPlayer(QUrl("file://1")));
        mediaPlayers.append(new MockMediaPlayer(QUrl("file://2")));
        mediaPlayers.append(new MockMediaPlayer(QUrl("file://3")));

        rearrangeVideoPlayers(layout, mediaPlayers);

        REQUIRE(layout.count() == 3);


        REQUIRE(layout.itemAtPosition(0, 0) != nullptr);
        REQUIRE(layout.itemAtPosition(0, 0)->widget() == mediaPlayers[0]->getVideoWidget());
        REQUIRE(layout.itemAtPosition(0, 1) != nullptr);
        REQUIRE(layout.itemAtPosition(0, 1)->widget() == mediaPlayers[1]->getVideoWidget());
        REQUIRE(layout.itemAtPosition(1, 0) != nullptr);
        REQUIRE(layout.itemAtPosition(1, 0)->widget() == mediaPlayers[2]->getVideoWidget());
        REQUIRE(layout.itemAtPosition(1, 1) == nullptr); // Check bounds
        REQUIRE(layout.itemAtPosition(0, 2) == nullptr);
    }

    SECTION("4 players") {
        mediaPlayers.append(new MockMediaPlayer(QUrl("file://1")));
        mediaPlayers.append(new MockMediaPlayer(QUrl("file://2")));
        mediaPlayers.append(new MockMediaPlayer(QUrl("file://3")));
        mediaPlayers.append(new MockMediaPlayer(QUrl("file://4")));

        rearrangeVideoPlayers(layout, mediaPlayers);

        REQUIRE(layout.count() == 4);


        REQUIRE(layout.itemAtPosition(0, 0) != nullptr);
        REQUIRE(layout.itemAtPosition(0, 0)->widget() == mediaPlayers[0]->getVideoWidget());
        REQUIRE(layout.itemAtPosition(0, 1) != nullptr);
        REQUIRE(layout.itemAtPosition(0, 1)->widget() == mediaPlayers[1]->getVideoWidget());
        REQUIRE(layout.itemAtPosition(1, 0) != nullptr);
        REQUIRE(layout.itemAtPosition(1, 0)->widget() == mediaPlayers[2]->getVideoWidget());
        REQUIRE(layout.itemAtPosition(1, 1) != nullptr);
        REQUIRE(layout.itemAtPosition(1, 1)->widget() == mediaPlayers[3]->getVideoWidget());
        REQUIRE(layout.itemAtPosition(0, 2) == nullptr); // Check bounds
        REQUIRE(layout.itemAtPosition(2, 0) == nullptr);
    }

    SECTION("5 players") {
        mediaPlayers.append(new MockMediaPlayer(QUrl("file://1")));
        mediaPlayers.append(new MockMediaPlayer(QUrl("file://2")));
        mediaPlayers.append(new MockMediaPlayer(QUrl("file://3")));
        mediaPlayers.append(new MockMediaPlayer(QUrl("file://4")));
        mediaPlayers.append(new MockMediaPlayer(QUrl("file://5")));


        rearrangeVideoPlayers(layout, mediaPlayers);

        REQUIRE(layout.count() == 5);


        REQUIRE(layout.itemAtPosition(0, 0) != nullptr);
        REQUIRE(layout.itemAtPosition(0, 0)->widget() == mediaPlayers[0]->getVideoWidget());
        REQUIRE(layout.itemAtPosition(0, 1) != nullptr);
        REQUIRE(layout.itemAtPosition(0, 1)->widget() == mediaPlayers[1]->getVideoWidget());
        REQUIRE(layout.itemAtPosition(0, 2) != nullptr);
        REQUIRE(layout.itemAtPosition(0, 2)->widget() == mediaPlayers[2]->getVideoWidget());
        REQUIRE(layout.itemAtPosition(1, 0) != nullptr);
        REQUIRE(layout.itemAtPosition(1, 0)->widget() == mediaPlayers[3]->getVideoWidget());
        REQUIRE(layout.itemAtPosition(1, 1) != nullptr);
        REQUIRE(layout.itemAtPosition(1, 1)->widget() == mediaPlayers[4]->getVideoWidget());
        REQUIRE(layout.itemAtPosition(1, 2) == nullptr); // Check bounds
        REQUIRE(layout.itemAtPosition(0, 3) == nullptr);
        REQUIRE(layout.itemAtPosition(2, 0) == nullptr);
    }
    for (MediaPlayerBase* player: mediaPlayers) {
        delete player;
    }
    mediaPlayers.clear();
}

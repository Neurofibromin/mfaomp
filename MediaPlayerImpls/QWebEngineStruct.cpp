#include "QWebEngineStruct.h"

QString QWebEngineStruct::generateHtmlContent(const QUrl& videoUrl) {
    return QString(R"(
            <!DOCTYPE html>
            <html>
            <head>
                <meta charset="utf-8">
                <title>QWebEngine Video Player</title>
                <style>
                    body { margin: 0; overflow: hidden; background-color: #222; }
                    video {
                        display: block;
                        width: 100vw;
                        height: 100vh;
                        object-fit: contain;
                        background-color: black;
                    }
                </style>
            </head>
            <body>
                <video id="mediaPlayerVideo" controls loop>
                    <source src="%1" type="video/mp4">
                    Your browser does not support the video tag or the video format.
                </video>
                <script>
                    // Basic error handling for video element
                    const video = document.getElementById('mediaPlayerVideo');
                    if (video) {
                        video.onerror = function() {
                            console.error('Video loading error:', video.error.code, video.error.message);
                            // You could send a message back to C++ here if needed
                            // new QWebChannel().send('videoError', video.error.message);
                        };
                        video.onloadedmetadata = function() {
                            console.log('Video metadata loaded. Duration:', video.duration);
                        };
                    } else {
                        console.error('Video element not found!');
                    }
                </script>
            </body>
            </html>
        )").arg(videoUrl.toString()); // Use url.toString() to embed it in HTML

        /*
        QString(R"(
            <html>
            <body style="margin:0; background-color:black;">
                <video id="mediaPlayerVideo" width="100%" height="100%" controls>
                    <source src="%1" type="video/mp4">
                </video>
                <script>
                    function playVideo() {
                        document.getElementById('mediaPlayerVideo').play();
                    }
                    function pauseVideo() {
                        document.getElementById('mediaPlayerVideo').pause();
                    }
                    function setSpeed(rate) {
                        document.getElementById('mediaPlayerVideo').playbackRate = rate;
                    }
                    function setTime(t) {
                        document.getElementById('mediaPlayerVideo').currentTime = t / 1000.0;
                    }
                </script>
            </body>
            </html>
        )").arg(videoUrl.toString());
         */
}

float QWebEngineStruct::speed(float sp) {
    if (sp != 0.0) {
        QString script = QString("document.getElementById('mediaPlayerVideo').playbackRate = %1;").arg(sp);
        webView->page()->runJavaScript(script);
        lastKnownSpeed = sp;
        return sp;
    } else {
        QEventLoop loop;
        QTimer timer;
        timer.setSingleShot(true);
        timer.setInterval(100);
        float currentSpeed = lastKnownSpeed;
        webView->page()->runJavaScript("document.getElementById('mediaPlayerVideo').playbackRate;",
                                       [&currentSpeed, &loop](const QVariant& result) {
                                           if (result.isValid() && result.typeId() == QMetaType::Double) {
                                               currentSpeed = result.toFloat();
                                               qDebug() << "QWebEngineStruct: Fetched speed:" << currentSpeed;
                                           } else {
                                               qWarning() << "QWebEngineStruct: Failed to get speed or invalid result:" << result;
                                           }
                                           loop.quit();
                                       });
        QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
        timer.start();
        loop.exec();
        lastKnownSpeed = currentSpeed;
        return currentSpeed;
    }
}

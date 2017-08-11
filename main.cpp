#include "mainwindow.h"
#include <QApplication>
#include <pianoroll.h>
#include <QQuickWidget>
#include <midiplayer.h>
#include <QQmlApplicationEngine>
#include <QQmlContext>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
     MidiManager *manager = new MidiManager;
    MainWindow window(manager);
   // PianoRoll piano(&window);
    QQuickWidget *view = new QQuickWidget;
    MidiPlayer *player = new MidiPlayer;


    view->setSource(QUrl::fromLocalFile("C:/Users/Puter/documents/MidiInter/Main.qml"));
        view->rootContext()->setContextProperty("manager", manager);
    auto rootObject = view->rootObject();
    QObject::connect((QObject*)rootObject,SIGNAL(playNote(QString,QString)),player,SLOT(playNote(QString,QString)));
    QObject::connect((QObject*)rootObject,SIGNAL(updateMidi(int,int,int)),manager,SLOT(updateMidi(int,int,int)));
    QObject::connect((QObject*)rootObject,SIGNAL(playSong()),&window,SLOT(playSong()));
    QObject::connect((QObject*)rootObject,SIGNAL(on_PauseButton_clicked(int)),&window,SLOT(on_PauseButton_clicked(int)));



    view->show();
    window.show();

    return app.exec();
}

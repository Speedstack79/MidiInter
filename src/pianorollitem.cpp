#include "pianorollitem.h"
#include <src/velocityview.h>
#include <QDebug>


PianoRollItem::PianoRollItem()
{
    brush = (QColor(102, 179, 255));
    setCacheMode(QGraphicsItem::NoCache);
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable,true);
}

QRectF PianoRollItem::boundingRect() const
{

    return QRectF(0,0,width,keyHeight);
}
void PianoRollItem::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    QRectF rect = boundingRect();
    //const qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());
    QPen pen(Qt::black,0);
    painter->setPen(pen);

    painter->fillRect(rect,brush);
    painter->drawRect(rect);

}

void PianoRollItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{

    QGraphicsItem::mouseDoubleClickEvent(event);

}

void PianoRollItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{

    int yPos = event->lastScenePos().y()/keyHeight;
    this->setY(yPos*keyHeight);
    int xMove = 0;
    int yMove = 0;

    double colSpacing =pianoroll->tPQN *pianoroll->scaleFactor;
    int xPos = event->lastScenePos().x()/colSpacing;

    if(initXPos != this->x()){
           qDebug() << this->x();
           xMove = x() - initXPos;
           initXPos = x();
    }
    if(initYPos != this->y()){
           qDebug() << this->y();
           yMove = y() - initYPos;
           initYPos = y();
    }
    if(yMove != 0 || xMove != 0){
        pianoroll->notifyPianoRollItemMoved(xMove,yMove,this);
    }

    if (xPos < 0) {
        xPos = 0;
    }else if(xPos*colSpacing + noteEnd > pianoroll->totalDT){
        xPos = pianoroll->totalDT - noteEnd;
        colSpacing =1;
    }
    this->setX(xPos*colSpacing);
    //Perhaps a bad idea, but if a drag changes the yPos, play the new note
    if (lastYWithSound != yPos*keyHeight) {
        int note = 127 - yPos;
        pianoroll->playKeyboardNote(note,true);

        QTimer::singleShot(300,[=](){
            pianoroll->turnNoteOff(note);
        });
        lastYWithSound = yPos*keyHeight;
    }
}

void PianoRollItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    int yPos = event->lastScenePos().y()/keyHeight;
    double colSpacing =pianoroll->tPQN *pianoroll->scaleFactor;
    int xPos = event->lastScenePos().x()/colSpacing;
    lastXPos = xPos*colSpacing;
    lastYPos = yPos*keyHeight;
    initXPos = lastXPos;
    initYPos = lastYPos;
    lastYWithSound = lastYPos;
}

void PianoRollItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    int yPos = event->lastScenePos().y()/keyHeight;
    double colSpacing =pianoroll->tPQN *pianoroll->scaleFactor;
    int xPos = event->lastScenePos().x()/colSpacing;

    if (((yPos*keyHeight) != lastYPos) || ((xPos*colSpacing) != lastXPos)) {
        int note = 127 - (lastYPos/keyHeight);
       MidiManager::updateMidiDelete(noteStart,noteEnd,note,pianoroll->track->track);
       pianoroll->velocityView->updateItems(noteStart,70,note,false);
       pianoroll->velocityView->updateItems(x(),70,127 - yPos,true);
       MidiManager::updateMidiAdd(127 - yPos,70,this->x(),noteEnd,pianoroll->track->track);
       noteStart = this->x();
    }
}

void PianoRollItem::setBoundingRect(int _width){

    prepareGeometryChange();
    this->width = _width;

}

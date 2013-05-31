/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

//Own
#include "graphicsscene.h"
#include "boat.h"
#include "submarine.h"
#include "torpedo.h"
#include "bomb.h"
#include "pixmapitem.h"
#include "animationmanager.h"
#include "qanimationstate.h"
#include "progressitem.h"
#include "textinformationitem.h"
#include "manualtransition.h"

//Qt
#include <QtCore/QPropertyAnimation>
#include <QtCore/QSequentialAnimationGroup>
#include <QtCore/QParallelAnimationGroup>
#include <QtCore/QStateMachine>
#include <QtCore/QFinalState>
#include <QtCore/QPauseAnimation>
#include <QtWidgets/QAction>
#include <QtCore/QDir>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QGraphicsSceneMouseEvent>
#include <QtCore/QXmlStreamReader>

GraphicsScene::GraphicsScene(const QRect &rect, Mode mode)
    : QGraphicsScene(rect), boat(new Boat), mode(mode), machine(new QStateMachine(this)), lettersMovingState(0), gameFinishedState(0)
{
    PixmapItem *backgroundItem = new PixmapItem(QString("background"), rect);
    backgroundItem->setZValue(1);
    backgroundItem->setPos(0,0);
    addItem(backgroundItem);

    PixmapItem *surfaceItem1 = new PixmapItem(QString("surface"), rect);
    surfaceItem1->setZValue(10);
    addItem(surfaceItem1);
    surfaceItem1->setPos(0, sealLevel() - 20);

    PixmapItem *surfaceItem2 = new PixmapItem(QString("surface"), rect);
    surfaceItem2->setZValue(10);
    addItem(surfaceItem2);
    surfaceItem2->setPos(0, sealLevel() - 20);

    QPropertyAnimation *moveSurfaceAnim1 = new QPropertyAnimation(surfaceItem1, "x", this);
    moveSurfaceAnim1->setDuration(18000);
    moveSurfaceAnim1->setStartValue(-surfaceItem1->boundingRect().width());
    moveSurfaceAnim1->setEndValue(0);
    moveSurfaceAnim1->setLoopCount(-1);
    moveSurfaceAnim1->start();

    QPropertyAnimation *moveSurfaceAnim2 = new QPropertyAnimation(surfaceItem2, "x", this);
    moveSurfaceAnim2->setDuration(18000);
    moveSurfaceAnim2->setStartValue(0);
    moveSurfaceAnim2->setEndValue(surfaceItem2->boundingRect().width());
    moveSurfaceAnim2->setLoopCount(-1);
    moveSurfaceAnim2->start();

    //The item that display score and level
    progressItem = new ProgressItem(backgroundItem);

    textInformationItem = new TextInformationItem(backgroundItem);
    textInformationItem->hide();
    //We create the boat
    addItem(boat);
    boat->setPos(100, sealLevel() - boat->size().height());
    
    QPropertyAnimation *moveBoatAnim = new QPropertyAnimation(boat, "y", this);
    moveBoatAnim->setDuration(10000);
    moveBoatAnim->setStartValue(sealLevel() - boat->size().height());
    moveBoatAnim->setKeyValueAt(0.5, sealLevel() - boat->size().height() - 5);
    moveBoatAnim->setEndValue(sealLevel() - boat->size().height());
    moveBoatAnim->setLoopCount(-1);
    moveBoatAnim->start();

    //parse the xml that contain all data of the game
    QXmlStreamReader reader;
    QFile file(":data.xml");
    file.open(QIODevice::ReadOnly);
    reader.setDevice(&file);
    LevelDescription currentLevel;
    while (!reader.atEnd()) {
         reader.readNext();
         if (reader.tokenType() == QXmlStreamReader::StartElement) {
             if (reader.name() == "submarine") {
                 SubmarineDescription desc;
                 desc.name = reader.attributes().value("name").toString();
                 desc.points = reader.attributes().value("points").toString().toInt();
                 desc.type = reader.attributes().value("type").toString().toInt();
                 submarinesData.append(desc);
             } else if (reader.name() == "level") {
                 currentLevel.id = reader.attributes().value("id").toString().toInt();
                 currentLevel.name = reader.attributes().value("name").toString();
             } else if (reader.name() == "subinstance") {
                 currentLevel.submarines.append(qMakePair(reader.attributes().value("type").toString().toInt(), reader.attributes().value("nb").toString().toInt()));
             }
         } else if (reader.tokenType() == QXmlStreamReader::EndElement) {
            if (reader.name() == "level") {
                levelsData.insert(currentLevel.id, currentLevel);
                currentLevel.submarines.clear();
            }
         }
   }

    connect(&sensor, &QAccelerometer::readingChanged, this, &GraphicsScene::sensorDataChanged);
}

qreal GraphicsScene::sealLevel() const
{
    return height() / 3.21;
}

void GraphicsScene::setupScene()
{
    qreal xOffset = 50;
    qreal yOffset = 0;
    static const int nLetters = 10;
    static struct {
        char const *pix;
        qreal initX, initY;
        qreal destX, destY;
    } logoData[nLetters] = {
        {"s",   100, 300, 300, 150 },
        {"u",    10, 300, 350, 150 },
        {"b",    10, 300, 400, 120 },
        {"dash", 100, 300, 460, 150 },
        {"a",    100,  300, 350, 250 },
        {"t",     100,  300, 400, 250 },
        {"t2",    100,  300, 430, 250 },
        {"a2",    100,  300, 465, 250 },
        {"q",     100,  300, 510, 250 },
        {"excl",  100,  300, 570, 220 } };

    QSequentialAnimationGroup * lettersGroupMoving = new QSequentialAnimationGroup(this);
    lettersGroupMoving->addPause(1000);
    QParallelAnimationGroup * lettersGroupMoving2 = new QParallelAnimationGroup(lettersGroupMoving);
    QParallelAnimationGroup * lettersGroupFading = new QParallelAnimationGroup(this);

    for (int i = 0; i < nLetters; ++i) {
        PixmapItem *logo = new PixmapItem(QLatin1String(":/logo-") + logoData[i].pix, this);
        logo->setPos(2000, 2000);
        logo->setZValue(i + 3);
        //creation of the animations for moving letters
        QPropertyAnimation *fadeAnim = new QPropertyAnimation(logo, "opacity", lettersGroupMoving2);
        fadeAnim->setDuration(500);
        fadeAnim->setStartValue(0);
        fadeAnim->setEndValue(1);
        QPropertyAnimation *moveAnim = new QPropertyAnimation(logo, "pos", lettersGroupMoving2);
        moveAnim->setStartValue(QPointF(xOffset + logoData[i].initX, yOffset + logoData[i].initY));
        moveAnim->setEndValue(QPointF(xOffset + logoData[i].destX, yOffset + logoData[i].destY));
        moveAnim->setDuration(2000);
        moveAnim->setEasingCurve(QEasingCurve::OutElastic);
        //creation of the animations for fading out the letters
        fadeAnim = new QPropertyAnimation(logo, "opacity", lettersGroupFading);
        fadeAnim->setDuration(500);
        fadeAnim->setStartValue(1);
        fadeAnim->setEndValue(0);
        fadeAnim->setEasingCurve(QEasingCurve::OutQuad);
    }

    //This state is when the player is playing
    playState = new PlayState(this, machine);

    //Final state
    QFinalState *final = new QFinalState(machine);

    //Animation when the player enter in the game
    lettersMovingState = new QAnimationState(machine);
    lettersMovingState->setAnimation(lettersGroupMoving);

    //Animation when the welcome screen disappear
    QAnimationState *lettersFadingState = new QAnimationState(machine);
    lettersFadingState->setAnimation(lettersGroupFading);

    //if new game then we fade out the welcome screen and start playing
    lettersMovingState->addTransition(new ManualTransition(lettersFadingState));
    lettersFadingState->addTransition(lettersFadingState, SIGNAL(animationFinished()), playState);

    //The state we end up in when the game is finished, and score is showing:
    gameFinishedState = new QState(machine);
    playState->addTransition(playState, SIGNAL(finished()), lettersMovingState);

    //Welcome screen is the initial state
    machine->setInitialState(lettersMovingState);

    machine->start();

    //We reach the final state, then we quit
    connect(machine, SIGNAL(finished()), qApp, SLOT(quit()));
}

void GraphicsScene::addItem(Bomb *bomb)
{
    bombs.insert(bomb);
    connect(bomb,SIGNAL(bombExecutionFinished()),this, SLOT(onBombExecutionFinished()));
    QGraphicsScene::addItem(bomb);
}

void GraphicsScene::addItem(Torpedo *torpedo)
{
    torpedos.insert(torpedo);
    connect(torpedo,SIGNAL(torpedoExecutionFinished()),this, SLOT(onTorpedoExecutionFinished()));
    QGraphicsScene::addItem(torpedo);
}

void GraphicsScene::addItem(SubMarine *submarine)
{
    submarines.insert(submarine);
    connect(submarine,SIGNAL(subMarineExecutionFinished()),this, SLOT(onSubMarineExecutionFinished()));
    QGraphicsScene::addItem(submarine);
}

void GraphicsScene::addItem(QGraphicsItem *item)
{
    QGraphicsScene::addItem(item);
}

void GraphicsScene::onBombExecutionFinished()
{
    Bomb *bomb = qobject_cast<Bomb *>(sender());
    bombs.remove(bomb);
    bomb->deleteLater();
    if (boat)
        boat->setBombsLaunched(boat->bombsLaunched() - 1);
}

void GraphicsScene::onTorpedoExecutionFinished()
{
    Torpedo *torpedo = qobject_cast<Torpedo *>(sender());
    torpedos.remove(torpedo);
    torpedo->deleteLater();
}

void GraphicsScene::onSubMarineExecutionFinished()
{
    SubMarine *submarine = qobject_cast<SubMarine *>(sender());
    submarines.remove(submarine);
    if (submarines.count() == 0)
        emit allSubMarineDestroyed(submarine->points());
    else
        emit subMarineDestroyed(submarine->points());
    submarine->deleteLater();
}

void GraphicsScene::clearScene()
{
    foreach (SubMarine *sub, submarines) {
        sub->destroy();
        sub->deleteLater();
    }

    foreach (Torpedo *torpedo, torpedos) {
        torpedo->destroy();
        torpedo->deleteLater();
    }

    foreach (Bomb *bomb, bombs) {
        bomb->destroy();
        bomb->deleteLater();
    }

    submarines.clear();
    bombs.clear();
    torpedos.clear();

    AnimationManager::self()->unregisterAllAnimations();
}

void GraphicsScene::sensorDataChanged()
{
    QAccelerometerReading *r = sensor.reading();
    if (r->y() < 0) {
        // left
        boat->setX(qMax(0., boat->x() - qMin(-r->y() * 3., 4.)));
    } else {
        // right
        boat->setX(qMin(qreal(boat->x() + qMin(r->y() * 3., 4.)), qreal(sceneRect().width() - boat->boundingRect().width())));
    }
}

void GraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    QGraphicsScene::mousePressEvent(mouseEvent);
    mouseEvent->setAccepted(true);

    ManualTransition::trigger(machine);
    ManualTransition::trigger(boat->machine);
    if (playState->submachine)
        ManualTransition::trigger(playState->submachine);
}

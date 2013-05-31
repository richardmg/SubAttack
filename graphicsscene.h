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

#ifndef __GRAPHICSSCENE__H__
#define __GRAPHICSSCENE__H__

//Qt
#include <QtWidgets/QGraphicsScene>
#include <QtCore/QSet>
#include <QtCore/QState>
#include <QtSensors>

#include "qanimationstate.h"
#include "states.h"

class Boat;
class SubMarine;
class Torpedo;
class Bomb;
class PixmapItem;
class ProgressItem;
class TextInformationItem;
QT_BEGIN_NAMESPACE
class QAction;
QT_END_NAMESPACE

class GraphicsScene : public QGraphicsScene
{
Q_OBJECT
public:
    enum Mode {
        Big = 0,
        Small
    };

    struct SubmarineDescription {
        int type;
        int points;
        QString name;
    };

    struct LevelDescription {
        int id;
        QString name;
        QList<QPair<int,int> > submarines;
    };

    GraphicsScene(const QRect &rect, Mode mode = Big);
    qreal sealLevel() const;
    void setupScene();
    void addItem(Bomb *bomb);
    void addItem(Torpedo *torpedo);
    void addItem(SubMarine *submarine);
    void addItem(QGraphicsItem *item);
    void clearScene();

    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    
    Boat *boat;
    QAccelerometer sensor;

signals:
    void subMarineDestroyed(int);
    void allSubMarineDestroyed(int);
    void fireBomb();

private slots:
    void onBombExecutionFinished();
    void onTorpedoExecutionFinished();
    void onSubMarineExecutionFinished();
    void sensorDataChanged();

private:
    Mode mode;
    ProgressItem *progressItem;
    TextInformationItem *textInformationItem;
    QSet<SubMarine *> submarines;
    QSet<Bomb *> bombs;
    QSet<Torpedo *> torpedos;
    QVector<SubmarineDescription> submarinesData;
    QHash<int, LevelDescription> levelsData;

    QStateMachine *machine;
    QAnimationState *lettersMovingState;
    PlayState *playState;
    QState *gameFinishedState;

    friend class PauseState;
    friend class PlayState;
    friend class LevelState;
    friend class LostState;
    friend class WinState;
    friend class WinTransition;
    friend class UpdateScoreTransition;
};

#endif //__GRAPHICSSCENE__H__


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
#include "mainwindow.h"
#include "graphicsscene.h"

//Qt
#include <QGraphicsView>
#include <QApplication>
#include <QMenu>
#include <QMenuBar>
#include <QLayout>
#include <QScreen>

#ifndef QT_NO_OPENGL
# include <QtOpenGL/QtOpenGL>
#endif

MainWindow::MainWindow() : QMainWindow(0)
{
    // Check if we have artwork for the sreen size. Otherwise we construct a
    // scene of size 1024x768, and scale the view to fit the screen instead:
    QRect screenRect = QGuiApplication::primaryScreen()->geometry();
    if (screenRect.width() < screenRect.height())
        screenRect.setSize(QSize(screenRect.height(), screenRect.width()));

    QString folder = QString::number(screenRect.width()) + "x" + QString::number(screenRect.height());
    QFile artworkForScreenSize(QStringLiteral(":/") + folder + QDir::separator() + "background");
    
    if (artworkForScreenSize.exists()) {
        scene = new GraphicsScene(screenRect);
        view = new QGraphicsView(scene, this);
    } else {
        QRect sceneRect = QRect(0, 0, 1024, 768);
        scene = new GraphicsScene(sceneRect);
        view = new QGraphicsView(scene, this);
        view->scale(qreal(screenRect.width()) / sceneRect.width(), qreal(screenRect.height()) / sceneRect.height());
    }

    view->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scene->setupScene();
#ifndef QT_NO_OPENGL
    view->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
#endif

    setCentralWidget(view);
    layout()->setSizeConstraint(QLayout::SetFixedSize);
}

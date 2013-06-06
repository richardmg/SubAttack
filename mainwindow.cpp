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
#include <QApplication>
#include <QMenu>
#include <QMenuBar>
#include <QLayout>
#include <QScreen>

#ifndef QT_NO_OPENGL
# include <QtOpenGL/QtOpenGL>
#endif

MainWindow::MainWindow(const QRect &sceneRect) : QGraphicsView()
{
    // Check if we have artwork for the sreen size. Otherwise we construct a
    // scene of size 1024x768, and scale the view to fit the screen instead:
    QString folder = QString::number(sceneRect.width()) + "x" + QString::number(sceneRect.height());
    QFile artworkForScreenSize(QStringLiteral(":/") + folder + QDir::separator() + "background");
    
    if (artworkForScreenSize.exists()) {
        scene = new GraphicsScene(sceneRect);
    } else {
        QRect sceneRect = QRect(0, 0, 1024, 768);
        scene = new GraphicsScene(sceneRect);
        scale(qreal(sceneRect.width()) / sceneRect.width(), qreal(sceneRect.height()) / sceneRect.height());
    }

    setScene(scene);
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scene->setupScene();
#ifndef QT_NO_OPENGL
    setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
#endif
}

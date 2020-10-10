/*
  This file is part of KDDockWidgets.

  SPDX-FileCopyrightText: 2019-2020 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Sérgio Martins <sergio.martins@kdab.com>

  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef KDDOCKWIDGETS_TESTS_UTILS_H
#define KDDOCKWIDGETS_TESTS_UTILS_H

#include "KDDockWidgets.h"
#include "DropIndicatorOverlayInterface_p.h"
#include "DockWidgetBase.h"
#include "Config.h"

#ifdef KDDOCKWIDGETS_QTWIDGETS
# include <QWidget>
# include <QToolButton>
#else
# include <QQuickView>
#endif

#include <QPointer>
#include <QVector>

#include <memory>

// clazy:excludeall=ctor-missing-parent-argument,missing-qobject-macro,range-loop,missing-typeinfo,detaching-member,function-args-by-ref,non-pod-global-static,reserve-candidates

namespace KDDockWidgets {

class FrameWidget;

namespace Tests {

enum ButtonAction {
    ButtonAction_None,
    ButtonAction_Press = 1,
    ButtonAction_Release = 2
};
Q_DECLARE_FLAGS(ButtonActions, ButtonAction)

struct DockDescriptor {
    Location loc;
    int relativeToIndex;
    QPointer<DockWidgetBase> createdDock;
    KDDockWidgets::AddingOption option;
};

struct EnsureTopLevelsDeleted
{
    EnsureTopLevelsDeleted()
        : m_originalFlags(Config::self().flags())
        , m_originalSeparatorThickness(Config::self().separatorThickness())
    {
    }

    ~EnsureTopLevelsDeleted()
    {
        if (topLevels().size() != 0) {
            qWarning() << "There's still top-level widgets present!" << topLevels();
        }

        // Other cleanup, since we use this class everywhere
        Config::self().setDockWidgetFactoryFunc(nullptr);
        Config::self().setFlags(m_originalFlags);
        Config::self().setSeparatorThickness(m_originalSeparatorThickness);
    }

    QWidgetList topLevels() const
    {
        QWidgetList result;
#ifdef KDDOCKWIDGETS_QTWIDGETS
        for (QWidget *w : qApp->topLevelWidgets()) {
            if (!qobject_cast<QToolButton*>(w))
                result << w;
        }
#endif
        return result;
    }

    const Config::Flags m_originalFlags;
    const int m_originalSeparatorThickness;
};

bool shouldBlacklistWarning(const QString &msg, const QString &category = {});

std::unique_ptr<MainWindowBase> createMainWindow(QSize sz = {600, 600},
                                                            KDDockWidgets::MainWindowOptions options = MainWindowOption_HasCentralFrame, const QString &name = {});



std::unique_ptr<KDDockWidgets::MainWindowBase> createMainWindow(QVector<DockDescriptor> &docks);

KDDockWidgets::DockWidgetBase *createDockWidget(const QString &name, QWidgetOrQuick *w,
                                                DockWidgetBase::Options options = {}, bool show = true,
                                                const QString &affinityName = {});
KDDockWidgets::DockWidgetBase *createDockWidget(const QString &name, QColor color = Qt::black);

class NonClosableWidget : public QWidget
{
public:
    Q_OBJECT
public:
    explicit NonClosableWidget(QWidget *parent = nullptr);
    ~NonClosableWidget() override;

protected:
    void closeEvent(QCloseEvent *event) override;
};

class MyWidget : public QWidgetOrQuick
{
public:
    explicit MyWidget(const QString &, QColor c = Qt::black);
    ~MyWidget() override;


    QSize minimumSizeHint() const override
    {
        return {100, 100};
    }

protected:
#ifdef KDDOCKWIDGETS_QTWIDGETS
    void paintEvent(QPaintEvent *) override;
#endif
private:
    QColor c;
};


#ifdef KDDOCKWIDGETS_QTQUICK
// Don't want to adapt dozens of locations so it compiles for QtQuick, so just typedef.
// the fact it's a button isn't important for the tests anyway

class QPushButton : public MyWidget
{
public:
    // use const char* to silence QtCreator static analizer warnings when using const char * in tst_docks.cpp
    // We don't have QT_NO_CAST_FROM_ASCII and still it complains, so use an indirection so I can read tst_docks while
    // porting to QtQuick without noise. Once the port is done feel free to change to QString.
    explicit QPushButton(const char *name)
        : MyWidget(QString::fromLatin1(name))
    {
    }
};

#endif

void doubleClickOn(QPoint globalPos, QWidget *receiver);
void pressOn(QPoint globalPos, QWidget *receiver);
void releaseOn(QPoint globalPos, QWidget *receiver);
void clickOn(QPoint globalPos, QWidget *receiver);
void moveMouseTo(QPoint globalDest, QWidget *receiver);

}

inline bool qpaPassedAsArgument(int argc, char *argv[])
{
    for (int i = 1; i < argc; ++i) {
        if (qstrcmp(argv[i], "-platform") == 0) {
            return true;
        }
    }

    return false;
}

}

Q_DECLARE_METATYPE(KDDockWidgets::Tests::DockDescriptor)


#endif


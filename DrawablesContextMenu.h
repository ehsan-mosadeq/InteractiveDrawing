#pragma once

#include <memory>

#include <QObject>

#include "DrawableActor.h"
#include "TextActor.h"

static class DrawablesContextMenu
{
    public:
    
    static void Show(QPoint const& pos,
        std::shared_ptr<DrawableActor> drawableActor,
        std::shared_ptr<TextActor> textActor,
        QWidget* parent)
    {
        if (!drawableActor->AnySelected())
            return;
        QMenu contextMenu(QObject::tr("Context menu"), parent);

        QAction sendToFront("Bring to Front", parent);
        QObject::connect(&sendToFront, &QAction::triggered, [=]()
            {
                drawableActor->BringSelectedToFront();
            });

        contextMenu.addAction(&sendToFront);
        QAction sendToBack(QObject::tr("Send to Back"), parent);
        QObject::connect(&sendToBack, &QAction::triggered, [=]()
            {
                drawableActor->SendSelectedToBack();
            });

        contextMenu.addAction(&sendToBack);

        QAction addText_(QObject::tr("Add Text"), parent);
        QObject::connect(&addText_, &QAction::triggered, [=]() { textActor->ShowTextEdit(pos); });
        contextMenu.addAction(&addText_);

        QAction editText_(QObject::tr("Edit Text"), parent);
        QObject::connect(&editText_, &QAction::triggered, textActor.get(), &TextActor::EditText);
        contextMenu.addAction(&editText_);

        QAction deleteSelected("Delete", parent);
        QObject::connect(&deleteSelected, &QAction::triggered,
            [=]() {
                drawableActor->DeletSelected();
                textActor->DeleteSelected();
            });

        contextMenu.addAction(&deleteSelected);
        contextMenu.exec(parent->mapToGlobal(pos));
    }
};
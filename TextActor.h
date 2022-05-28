#pragma once

#include <algorithm>
#include <memory>
#include <vector>

#include <QObject>
#include "Drawables.h"

class TextActor : public QObject
{
    Q_OBJECT
    public:
    using TextRepPtr = std::shared_ptr<TextRep>;
    using SceneMapperPtr = std::shared_ptr<SceneMapper>;

    TextActor(QTextEdit* textEdit, SceneMapperPtr mapper) :
        m_textEdit(textEdit),
        m_mapper(mapper)
    {
        m_textEdit->installEventFilter(this);
    }

    bool eventFilter(QObject* object, QEvent* event)
    {
        if (event->type() == QEvent::FocusOut)
        {
            if (object == m_textEdit)
            {
                m_textEdit->hide();
                auto txt = m_textEdit->toPlainText();

                if (m_underEditText == nullptr)
                {
                    auto pos = m_mapper->MapToScene(m_textEdit->pos());
                    auto textRep = DrawablesInit::InitText(pos, txt);
                    m_texts.push_back(textRep);
                    emit TextCreated(textRep);
                }
                else
                {
                    m_underEditText->SetText(txt);
                    m_underEditText = nullptr;
                }
            }
        }
        return false;
    }

    void ShowTextEdit(QPoint const& pos)
    {
        m_textEdit->clear();
        if (m_underEditText != nullptr)
            m_textEdit->setText(m_underEditText->GetText());

        m_textEdit->moveCursor(QTextCursor::MoveOperation::End);
        m_textEdit->move(pos);
        m_textEdit->show();
        m_textEdit->setFocus();
    }

    void EditText()
    {
        auto textIt = std::find_if(m_texts.cbegin(), m_texts.cend(),
            [](TextRepPtr txt) {return txt->GetModel()->IsSelected(); });

        if (textIt == m_texts.cend())
            return;

        auto textRep = *textIt;
        m_underEditText = textRep;
        ShowTextEdit(m_mapper->MapFromScene(
            textRep->GetModel()->GetPosition().toPoint()).toPoint());
    }

    void DeleteSelected()
    {
        auto selectedText = std::find_if(m_texts.cbegin(), m_texts.cend(),
            [](const TextRepPtr& txt) {
                return txt->GetModel()->IsSelected();
            });

        if (selectedText == m_texts.cend())
            return;

        m_texts.erase(selectedText);
    }

    signals:
    void TextCreated(TextRepPtr textRep);

    private:
    std::vector<TextRepPtr> m_texts;
    TextRepPtr m_underEditText = nullptr;
    QTextEdit* m_textEdit;
    SceneMapperPtr m_mapper;
};
// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2017-2019 Alejandro Sirgo Rica & Contributors

#include "textwidget.h"

#include <QEvent>
#include <QKeyEvent>
#include <QMouseEvent>

namespace {
constexpr int DRAG_MARGIN = 6;
}

TextWidget::TextWidget(QWidget* parent)
  : QTextEdit(parent)
{
    setStyleSheet(QStringLiteral("TextWidget { background: transparent; }"));
    connect(this, &TextWidget::textChanged, this, &TextWidget::adjustSize);
    connect(this, &TextWidget::textChanged, this, &TextWidget::emitTextUpdated);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setContextMenuPolicy(Qt::NoContextMenu);
}

bool TextWidget::isOnDragMargin(const QPoint& pos) const
{
    const QRect r = rect();
    return pos.x() - r.left() < DRAG_MARGIN ||
           r.right() - pos.x() < DRAG_MARGIN ||
           pos.y() - r.top() < DRAG_MARGIN ||
           r.bottom() - pos.y() < DRAG_MARGIN;
}

bool TextWidget::event(QEvent* e)
{
    if (e->type() == QEvent::ShortcutOverride) {
        auto* keyEvent = static_cast<QKeyEvent*>(e);
        if (keyEvent->key() == Qt::Key_Escape) {
            keyEvent->accept();
            return true;
        }
    }

    return QTextEdit::event(e);
}

void TextWidget::keyPressEvent(QKeyEvent* e)
{
    if (e->key() == Qt::Key_Escape) {
        emit editingFinished();
        e->accept();
        return;
    }

    QTextEdit::keyPressEvent(e);
}

void TextWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton &&
        isOnDragMargin(event->pos())) {
        m_dragging = true;
        m_dragOffset = event->pos();
        setCursor(Qt::ClosedHandCursor);
        // Keep receiving mouse moves even if the cursor leaves the widget
        // while it is being dragged.
        grabMouse();
        event->accept();
        return;
    }
    QTextEdit::mousePressEvent(event);
}

void TextWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (m_dragging) {
        if (event->buttons() & Qt::LeftButton) {
            const QPoint newPos = mapToParent(event->pos() - m_dragOffset);
            move(newPos);
            emit dragged(newPos);
        }
        event->accept();
        return;
    }
    if (event->buttons() == Qt::NoButton) {
        setCursor(isOnDragMargin(event->pos()) ? Qt::OpenHandCursor
                                                : Qt::IBeamCursor);
    }
    QTextEdit::mouseMoveEvent(event);
}

void TextWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (m_dragging && event->button() == Qt::LeftButton) {
        m_dragging = false;
        releaseMouse();
        unsetCursor();
        event->accept();
        return;
    }
    QTextEdit::mouseReleaseEvent(event);
}

void TextWidget::showEvent(QShowEvent* e)
{
    QFont font;
    QFontMetrics fm(font);
    setFixedWidth(fm.lineSpacing() * 6);
    setFixedHeight(fm.lineSpacing() * 2.5);
    m_baseSize = size();
    m_minSize = m_baseSize;
    QTextEdit::showEvent(e);
    adjustSize();
}

void TextWidget::resizeEvent(QResizeEvent* e)
{
    m_minSize.setHeight(qMin(m_baseSize.height(), height()));
    m_minSize.setWidth(qMin(m_baseSize.width(), width()));
    QTextEdit::resizeEvent(e);
}

void TextWidget::setFont(const QFont& f)
{
    QTextEdit::setFont(f);
    adjustSize();
}

void TextWidget::setAlignment(Qt::AlignmentFlag alignment)
{
    QTextEdit::setAlignment(alignment);
    adjustSize();
}
void TextWidget::setTextColor(const QColor& c)
{
    QString s(
      QStringLiteral("TextWidget { background: transparent; color: %1; }"));
    setStyleSheet(s.arg(c.name()));
}

void TextWidget::adjustSize()
{
    QString&& text = this->toPlainText();

    QFontMetrics fm(font());
    QRect bounds = fm.boundingRect(QRect(), 0, text);
    int pixelsWide = bounds.width() + fm.lineSpacing();
    int pixelsHigh = bounds.height() * 1.15 + fm.lineSpacing();
    if (pixelsWide < m_minSize.width()) {
        pixelsWide = m_minSize.width();
    }
    if (pixelsHigh < m_minSize.height()) {
        pixelsHigh = m_minSize.height();
    }

    this->setFixedSize(pixelsWide, pixelsHigh);
}

void TextWidget::emitTextUpdated()
{
    emit textUpdated(this->toPlainText());
}

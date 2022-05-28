#include <QtWidgets>

#include "renderarea.h"
#include "window.h"


const int IdRole = Qt::UserRole;

Window::Window()
{
    this->setMinimumHeight(600);
    this->setMinimumWidth(1000);

    renderArea = new RenderArea;
    
    shapeComboBox = new QComboBox;
    shapeComboBox->addItem(tr("Rectangle"), (int)RenderArea::Shape::Rect);
    shapeComboBox->addItem(tr("Ellipse"), (int)RenderArea::Shape::Ellipse);
    shapeComboBox->addItem(tr("Line"), (int)RenderArea::Shape::Line);
    shapeComboBox->addItem(tr("Text"), (int)RenderArea::Shape::Text);
    shapeComboBox->addItem(tr("Node"), (int)RenderArea::Shape::Node);
    shapeComboBox->addItem(tr("Free"), (int)RenderArea::Shape::None);

    aboutLabel = new QLabel(tr(
        "                 \n"
        "Right Button: Draw\n"
        "                 \n"
        "Left Button: Move the Plane\n"
        "                 \n"
        "Ctrl + Right Button: Menu\n"
        "                 \n"
        "Ctrl + Left Button: Zoom\n"
        "                 \n"
        "Del: Delete the Selected Shape\n"
    ));
    aboutLabel->setStyleSheet("border: 3px solid blue;");
    shapeLabel = new QLabel(tr("&Shape:"));
    shapeLabel->setBuddy(shapeComboBox);

    penWidthSpinBox = new QSpinBox;
    penWidthSpinBox->setRange(0, 20);
    penWidthSpinBox->setSpecialValueText(tr("cosmetic pen"));

    penWidthLabel = new QLabel(tr("Pen &Width:"));
    penWidthLabel->setBuddy(penWidthSpinBox);

    penStyleComboBox = new QComboBox;
    penStyleComboBox->addItem(tr("Solid"), static_cast<int>(Qt::SolidLine));
    penStyleComboBox->addItem(tr("Dash"), static_cast<int>(Qt::DashLine));
    penStyleComboBox->addItem(tr("Dot"), static_cast<int>(Qt::DotLine));
    penStyleComboBox->addItem(tr("Dash Dot"), static_cast<int>(Qt::DashDotLine));
    penStyleComboBox->addItem(tr("Dash Dot Dot"), static_cast<int>(Qt::DashDotDotLine));
    penStyleComboBox->addItem(tr("None"), static_cast<int>(Qt::NoPen));

    penStyleLabel = new QLabel(tr("&Pen Style:"));
    penStyleLabel->setBuddy(penStyleComboBox);

    brushStyleComboBox = new QComboBox;
    brushStyleComboBox->addItem(tr("Linear Gradient"),
            static_cast<int>(Qt::LinearGradientPattern));
    brushStyleComboBox->addItem(tr("Radial Gradient"),
            static_cast<int>(Qt::RadialGradientPattern));
    brushStyleComboBox->addItem(tr("Conical Gradient"),
            static_cast<int>(Qt::ConicalGradientPattern));
    brushStyleComboBox->addItem(tr("Texture"), static_cast<int>(Qt::TexturePattern));
    brushStyleComboBox->addItem(tr("Solid"), static_cast<int>(Qt::SolidPattern));
    brushStyleComboBox->addItem(tr("Horizontal"), static_cast<int>(Qt::HorPattern));
    brushStyleComboBox->addItem(tr("Vertical"), static_cast<int>(Qt::VerPattern));
    brushStyleComboBox->addItem(tr("Cross"), static_cast<int>(Qt::CrossPattern));
    brushStyleComboBox->addItem(tr("Backward Diagonal"), static_cast<int>(Qt::BDiagPattern));
    brushStyleComboBox->addItem(tr("Forward Diagonal"), static_cast<int>(Qt::FDiagPattern));
    brushStyleComboBox->addItem(tr("Diagonal Cross"), static_cast<int>(Qt::DiagCrossPattern));
    brushStyleComboBox->addItem(tr("None"), static_cast<int>(Qt::NoBrush));

    brushStyleLabel = new QLabel(tr("&Brush:"));
    brushStyleLabel->setBuddy(brushStyleComboBox);

    connect(shapeComboBox, &QComboBox::activated,
            this, &Window::shapeChanged);
    connect(penWidthSpinBox, &QSpinBox::valueChanged,
            this, &Window::penChanged);
    connect(penStyleComboBox, &QComboBox::activated,
            this, &Window::penChanged);
    connect(brushStyleComboBox, &QComboBox::activated,
            this, &Window::brushChanged);

    auto mainLayout = new QHBoxLayout;
    auto ctrlsLayout = new QGridLayout;
    mainLayout->addLayout(ctrlsLayout);
    mainLayout->addWidget(renderArea);
    mainLayout->setStretch(0, 1);
    mainLayout->setStretch(1, 5);

    ctrlsLayout->addWidget(shapeLabel, 0, 0, Qt::AlignLeft);
    ctrlsLayout->addWidget(shapeComboBox, 0, 1);
    ctrlsLayout->addWidget(penWidthLabel, 1, 0, Qt::AlignLeft);
    ctrlsLayout->addWidget(penWidthSpinBox, 1, 1);
    ctrlsLayout->addWidget(penStyleLabel, 2, 0, Qt::AlignLeft);
    ctrlsLayout->addWidget(penStyleComboBox, 2, 1);
    ctrlsLayout->addWidget(brushStyleLabel, 3, 0, Qt::AlignLeft);
    ctrlsLayout->addWidget(brushStyleComboBox, 3, 1);
    ctrlsLayout->addWidget(aboutLabel, 4, 0, 1, 2);
    ctrlsLayout->setRowStretch(5, 8);

    setLayout(mainLayout);
    penChanged();
    brushChanged();
    setWindowTitle(tr("Interactive Drawing"));
    renderArea->setAction(RenderArea::Shape::Rect);
}

void Window::shapeChanged()
{
    RenderArea::Shape action = RenderArea::Shape(shapeComboBox->itemData(
            shapeComboBox->currentIndex(), IdRole).toInt());
    renderArea->setAction(action);
}

void Window::penChanged()
{
    int width = penWidthSpinBox->value();
    Qt::PenStyle style = Qt::PenStyle(penStyleComboBox->itemData(
            penStyleComboBox->currentIndex(), IdRole).toInt());

    renderArea->setPen(QPen(Qt::blue, width, style));
}

void Window::brushChanged()
{
    Qt::BrushStyle style = Qt::BrushStyle(brushStyleComboBox->itemData(

            brushStyleComboBox->currentIndex(), IdRole).toInt());

    if (style == Qt::LinearGradientPattern) {
        QLinearGradient linearGradient(0, 0, 100, 100);
        linearGradient.setColorAt(0.0, Qt::white);
        linearGradient.setColorAt(0.2, Qt::green);
        linearGradient.setColorAt(1.0, Qt::black);
        renderArea->setBrush(linearGradient);

    } else if (style == Qt::RadialGradientPattern) {
        QRadialGradient radialGradient(50, 50, 50, 70, 70);
        radialGradient.setColorAt(0.0, Qt::white);
        radialGradient.setColorAt(0.2, Qt::green);
        radialGradient.setColorAt(1.0, Qt::black);
        renderArea->setBrush(radialGradient);
    } else if (style == Qt::ConicalGradientPattern) {
        QConicalGradient conicalGradient(50, 50, 150);
        conicalGradient.setColorAt(0.0, Qt::white);
        conicalGradient.setColorAt(0.2, Qt::green);
        conicalGradient.setColorAt(1.0, Qt::black);
        renderArea->setBrush(conicalGradient);

    } else if (style == Qt::TexturePattern) {
        renderArea->setBrush(QBrush(QPixmap(":/images/brick.png")));

    } else {
        renderArea->setBrush(QBrush(Qt::green, style));
    }
}


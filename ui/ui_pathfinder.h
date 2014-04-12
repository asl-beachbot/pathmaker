/********************************************************************************
** Form generated from reading UI file 'pathfinder.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef PATHFINDER_H
#define PATHFINDER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QGraphicsView>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QMainWindow>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout;
    QGraphicsView *view;
    QVBoxLayout *verticalLayout;
    QVBoxLayout *verticalLayout_3;
    QSpacerItem *verticalSpacer_2;
    QCheckBox *ssCheckBox;
    QSpacerItem *verticalSpacer;
    QLabel *label;
    QDoubleSpinBox *doubleSpinBox;
    QPushButton *roundCornersButton;
    QPushButton *exportButton;
    QHBoxLayout *horizontalLayout_2;
    QLabel *mouse_xy;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(887, 641);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        verticalLayout_2 = new QVBoxLayout(centralwidget);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
        view = new QGraphicsView(centralwidget);
        view->setObjectName(QString::fromUtf8("view"));
        view->setEnabled(true);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(100);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(view->sizePolicy().hasHeightForWidth());
        view->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(view);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_3->addItem(verticalSpacer_2);

        ssCheckBox = new QCheckBox(centralwidget);
        ssCheckBox->setObjectName(QString::fromUtf8("ssCheckBox"));
        QFont font;
        font.setPointSize(8);
        ssCheckBox->setFont(font);
        ssCheckBox->setChecked(true);

        verticalLayout_3->addWidget(ssCheckBox);

        verticalSpacer = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Maximum);

        verticalLayout_3->addItem(verticalSpacer);

        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));
        QFont font1;
        font1.setPointSize(7);
        label->setFont(font1);

        verticalLayout_3->addWidget(label);

        doubleSpinBox = new QDoubleSpinBox(centralwidget);
        doubleSpinBox->setObjectName(QString::fromUtf8("doubleSpinBox"));

        verticalLayout_3->addWidget(doubleSpinBox);


        verticalLayout->addLayout(verticalLayout_3);

        roundCornersButton = new QPushButton(centralwidget);
        roundCornersButton->setObjectName(QString::fromUtf8("roundCornersButton"));

        verticalLayout->addWidget(roundCornersButton);

        exportButton = new QPushButton(centralwidget);
        exportButton->setObjectName(QString::fromUtf8("exportButton"));

        verticalLayout->addWidget(exportButton);


        horizontalLayout->addLayout(verticalLayout);

        horizontalLayout->setStretch(1, 3);

        verticalLayout_2->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        mouse_xy = new QLabel(centralwidget);
        mouse_xy->setObjectName(QString::fromUtf8("mouse_xy"));

        horizontalLayout_2->addWidget(mouse_xy);


        verticalLayout_2->addLayout(horizontalLayout_2);

        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
        ssCheckBox->setText(QApplication::translate("MainWindow", "Show Straight Seleton", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MainWindow", "Polygon rounding radius", 0, QApplication::UnicodeUTF8));
        roundCornersButton->setText(QApplication::translate("MainWindow", "Round Corners", 0, QApplication::UnicodeUTF8));
        exportButton->setText(QApplication::translate("MainWindow", "Export Poly", 0, QApplication::UnicodeUTF8));
        mouse_xy->setText(QApplication::translate("MainWindow", "Coordinates (click and drag to show)", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // PATHFINDER_H

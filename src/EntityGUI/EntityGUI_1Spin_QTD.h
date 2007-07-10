/********************************************************************************
** Form generated from reading ui file 'EntityGUI_1Spin_QTD.ui'
**
** Created: Tue Jul 10 10:27:13 2007
**      by: Qt User Interface Compiler version 4.2.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef ENTITYGUI_1SPIN_QTD_H
#define ENTITYGUI_1SPIN_QTD_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QWidget>
#include <Qt3Support/Q3MimeSourceFactory>

class Ui_EntityGUI_1Spin_QTD
{
public:
    QGridLayout *gridLayout;
    QGroupBox *GroupBox1;
    QGridLayout *gridLayout1;
    QGridLayout *gridLayout2;
    QGridLayout *gridLayout3;
    QLabel *TextLabel1;
    QSpinBox *SpinBox1;
    QSpacerItem *spacerItem;
    QGridLayout *gridLayout4;
    QGridLayout *gridLayout5;
    QPushButton *buttonRedo;
    QPushButton *buttonUndo;
    QPushButton *buttonApply;
    QSpacerItem *spacerItem1;

    void setupUi(QWidget *EntityGUI_1Spin_QTD)
    {
    EntityGUI_1Spin_QTD->setObjectName(QString::fromUtf8("EntityGUI_1Spin_QTD"));
    gridLayout = new QGridLayout(EntityGUI_1Spin_QTD);
    gridLayout->setSpacing(6);
    gridLayout->setMargin(0);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    GroupBox1 = new QGroupBox(EntityGUI_1Spin_QTD);
    GroupBox1->setObjectName(QString::fromUtf8("GroupBox1"));
    gridLayout1 = new QGridLayout(GroupBox1);
    gridLayout1->setSpacing(6);
    gridLayout1->setMargin(11);
    gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
    gridLayout2 = new QGridLayout();
    gridLayout2->setSpacing(6);
    gridLayout2->setMargin(0);
    gridLayout2->setObjectName(QString::fromUtf8("gridLayout2"));
    gridLayout3 = new QGridLayout();
    gridLayout3->setSpacing(6);
    gridLayout3->setMargin(0);
    gridLayout3->setObjectName(QString::fromUtf8("gridLayout3"));
    TextLabel1 = new QLabel(GroupBox1);
    TextLabel1->setObjectName(QString::fromUtf8("TextLabel1"));
    QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(0), static_cast<QSizePolicy::Policy>(0));
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(TextLabel1->sizePolicy().hasHeightForWidth());
    TextLabel1->setSizePolicy(sizePolicy);
    TextLabel1->setWordWrap(false);

    gridLayout3->addWidget(TextLabel1, 0, 0, 1, 1);

    SpinBox1 = new QSpinBox(GroupBox1);
    SpinBox1->setObjectName(QString::fromUtf8("SpinBox1"));
    QSizePolicy sizePolicy1(static_cast<QSizePolicy::Policy>(7), static_cast<QSizePolicy::Policy>(0));
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(SpinBox1->sizePolicy().hasHeightForWidth());
    SpinBox1->setSizePolicy(sizePolicy1);

    gridLayout3->addWidget(SpinBox1, 0, 1, 1, 1);

    spacerItem = new QSpacerItem(0, 82, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout3->addItem(spacerItem, 3, 1, 1, 1);


    gridLayout2->addLayout(gridLayout3, 0, 0, 1, 1);

    gridLayout4 = new QGridLayout();
    gridLayout4->setSpacing(6);
    gridLayout4->setMargin(0);
    gridLayout4->setObjectName(QString::fromUtf8("gridLayout4"));
    gridLayout5 = new QGridLayout();
    gridLayout5->setSpacing(6);
    gridLayout5->setMargin(0);
    gridLayout5->setObjectName(QString::fromUtf8("gridLayout5"));
    buttonRedo = new QPushButton(GroupBox1);
    buttonRedo->setObjectName(QString::fromUtf8("buttonRedo"));

    gridLayout5->addWidget(buttonRedo, 0, 1, 1, 1);

    buttonUndo = new QPushButton(GroupBox1);
    buttonUndo->setObjectName(QString::fromUtf8("buttonUndo"));

    gridLayout5->addWidget(buttonUndo, 0, 0, 1, 1);


    gridLayout4->addLayout(gridLayout5, 1, 0, 1, 1);

    buttonApply = new QPushButton(GroupBox1);
    buttonApply->setObjectName(QString::fromUtf8("buttonApply"));

    gridLayout4->addWidget(buttonApply, 0, 0, 1, 1);

    spacerItem1 = new QSpacerItem(0, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout4->addItem(spacerItem1, 2, 0, 1, 1);


    gridLayout2->addLayout(gridLayout4, 0, 1, 1, 1);


    gridLayout1->addLayout(gridLayout2, 0, 0, 1, 1);


    gridLayout->addWidget(GroupBox1, 0, 0, 1, 1);


    retranslateUi(EntityGUI_1Spin_QTD);

    QSize size(255, 112);
    size = size.expandedTo(EntityGUI_1Spin_QTD->minimumSizeHint());
    EntityGUI_1Spin_QTD->resize(size);


    QMetaObject::connectSlotsByName(EntityGUI_1Spin_QTD);
    } // setupUi

    void retranslateUi(QWidget *EntityGUI_1Spin_QTD)
    {
    EntityGUI_1Spin_QTD->setWindowTitle(QApplication::translate("EntityGUI_1Spin_QTD", "EntityGUI_1Spin_QTD", 0, QApplication::UnicodeUTF8));
    GroupBox1->setTitle(QApplication::translate("EntityGUI_1Spin_QTD", "Values", 0, QApplication::UnicodeUTF8));
    TextLabel1->setText(QApplication::translate("EntityGUI_1Spin_QTD", "TL1", 0, QApplication::UnicodeUTF8));
    buttonRedo->setText(QApplication::translate("EntityGUI_1Spin_QTD", "Redo", 0, QApplication::UnicodeUTF8));
    buttonUndo->setText(QApplication::translate("EntityGUI_1Spin_QTD", "Undo", 0, QApplication::UnicodeUTF8));
    buttonApply->setText(QApplication::translate("EntityGUI_1Spin_QTD", "Create", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(EntityGUI_1Spin_QTD);
    } // retranslateUi

};

namespace Ui {
    class EntityGUI_1Spin_QTD: public Ui_EntityGUI_1Spin_QTD {};
} // namespace Ui

#endif // ENTITYGUI_1SPIN_QTD_H

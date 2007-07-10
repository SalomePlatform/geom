/********************************************************************************
** Form generated from reading ui file 'EntityGUI_1Sel_QTD.ui'
**
** Created: Tue Jul 10 10:38:07 2007
**      by: Qt User Interface Compiler version 4.2.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef ENTITYGUI_1SEL_QTD_H
#define ENTITYGUI_1SEL_QTD_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QWidget>
#include <Qt3Support/Q3MimeSourceFactory>

class Ui_EntityGUI_1Sel_QTD
{
public:
    QGridLayout *gridLayout;
    QGroupBox *GroupBox1;
    QGridLayout *gridLayout1;
    QGridLayout *gridLayout2;
    QGridLayout *gridLayout3;
    QSpacerItem *spacerItem;
    QPushButton *buttonApply;
    QGridLayout *gridLayout4;
    QPushButton *buttonRedo;
    QPushButton *buttonUndo;
    QGridLayout *gridLayout5;
    QLabel *TextLabel1;
    QSpacerItem *spacerItem1;
    QPushButton *PushButton1;
    QLineEdit *LineEdit1;

    void setupUi(QWidget *EntityGUI_1Sel_QTD)
    {
    EntityGUI_1Sel_QTD->setObjectName(QString::fromUtf8("EntityGUI_1Sel_QTD"));
    gridLayout = new QGridLayout(EntityGUI_1Sel_QTD);
    gridLayout->setSpacing(6);
    gridLayout->setMargin(0);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    GroupBox1 = new QGroupBox(EntityGUI_1Sel_QTD);
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
    spacerItem = new QSpacerItem(0, 163, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout3->addItem(spacerItem, 2, 0, 1, 1);

    buttonApply = new QPushButton(GroupBox1);
    buttonApply->setObjectName(QString::fromUtf8("buttonApply"));

    gridLayout3->addWidget(buttonApply, 0, 0, 1, 1);

    gridLayout4 = new QGridLayout();
    gridLayout4->setSpacing(6);
    gridLayout4->setMargin(0);
    gridLayout4->setObjectName(QString::fromUtf8("gridLayout4"));
    buttonRedo = new QPushButton(GroupBox1);
    buttonRedo->setObjectName(QString::fromUtf8("buttonRedo"));

    gridLayout4->addWidget(buttonRedo, 0, 1, 1, 1);

    buttonUndo = new QPushButton(GroupBox1);
    buttonUndo->setObjectName(QString::fromUtf8("buttonUndo"));

    gridLayout4->addWidget(buttonUndo, 0, 0, 1, 1);


    gridLayout3->addLayout(gridLayout4, 1, 0, 1, 1);


    gridLayout2->addLayout(gridLayout3, 0, 1, 1, 1);

    gridLayout5 = new QGridLayout();
    gridLayout5->setSpacing(6);
    gridLayout5->setMargin(0);
    gridLayout5->setObjectName(QString::fromUtf8("gridLayout5"));
    TextLabel1 = new QLabel(GroupBox1);
    TextLabel1->setObjectName(QString::fromUtf8("TextLabel1"));
    QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(0), static_cast<QSizePolicy::Policy>(0));
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(TextLabel1->sizePolicy().hasHeightForWidth());
    TextLabel1->setSizePolicy(sizePolicy);
    TextLabel1->setWordWrap(false);

    gridLayout5->addWidget(TextLabel1, 0, 0, 1, 1);

    spacerItem1 = new QSpacerItem(0, 180, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout5->addItem(spacerItem1, 1, 2, 1, 1);

    PushButton1 = new QPushButton(GroupBox1);
    PushButton1->setObjectName(QString::fromUtf8("PushButton1"));
    QSizePolicy sizePolicy1(static_cast<QSizePolicy::Policy>(0), static_cast<QSizePolicy::Policy>(0));
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(PushButton1->sizePolicy().hasHeightForWidth());
    PushButton1->setSizePolicy(sizePolicy1);

    gridLayout5->addWidget(PushButton1, 0, 1, 1, 1);

    LineEdit1 = new QLineEdit(GroupBox1);
    LineEdit1->setObjectName(QString::fromUtf8("LineEdit1"));
    QSizePolicy sizePolicy2(static_cast<QSizePolicy::Policy>(3), static_cast<QSizePolicy::Policy>(0));
    sizePolicy2.setHorizontalStretch(0);
    sizePolicy2.setVerticalStretch(0);
    sizePolicy2.setHeightForWidth(LineEdit1->sizePolicy().hasHeightForWidth());
    LineEdit1->setSizePolicy(sizePolicy2);

    gridLayout5->addWidget(LineEdit1, 0, 2, 1, 1);


    gridLayout2->addLayout(gridLayout5, 0, 0, 1, 1);


    gridLayout1->addLayout(gridLayout2, 0, 0, 1, 1);


    gridLayout->addWidget(GroupBox1, 0, 0, 1, 1);


    retranslateUi(EntityGUI_1Sel_QTD);

    QSize size(329, 112);
    size = size.expandedTo(EntityGUI_1Sel_QTD->minimumSizeHint());
    EntityGUI_1Sel_QTD->resize(size);


    QMetaObject::connectSlotsByName(EntityGUI_1Sel_QTD);
    } // setupUi

    void retranslateUi(QWidget *EntityGUI_1Sel_QTD)
    {
    EntityGUI_1Sel_QTD->setWindowTitle(QApplication::translate("EntityGUI_1Sel_QTD", "EntityGUI_1Sel_QTD", 0, QApplication::UnicodeUTF8));
    GroupBox1->setTitle(QApplication::translate("EntityGUI_1Sel_QTD", "Values", 0, QApplication::UnicodeUTF8));
    buttonApply->setText(QApplication::translate("EntityGUI_1Sel_QTD", "Create", 0, QApplication::UnicodeUTF8));
    buttonRedo->setText(QApplication::translate("EntityGUI_1Sel_QTD", "Redo", 0, QApplication::UnicodeUTF8));
    buttonUndo->setText(QApplication::translate("EntityGUI_1Sel_QTD", "Undo", 0, QApplication::UnicodeUTF8));
    TextLabel1->setText(QApplication::translate("EntityGUI_1Sel_QTD", "TL1", 0, QApplication::UnicodeUTF8));
    PushButton1->setText(QString());
    Q_UNUSED(EntityGUI_1Sel_QTD);
    } // retranslateUi

};

namespace Ui {
    class EntityGUI_1Sel_QTD: public Ui_EntityGUI_1Sel_QTD {};
} // namespace Ui

#endif // ENTITYGUI_1SEL_QTD_H

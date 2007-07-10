/********************************************************************************
** Form generated from reading ui file 'EntityGUI_4Spin_QTD.ui'
**
** Created: Tue Jul 10 10:35:47 2007
**      by: Qt User Interface Compiler version 4.2.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef ENTITYGUI_4SPIN_QTD_H
#define ENTITYGUI_4SPIN_QTD_H

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

class Ui_EntityGUI_4Spin_QTD
{
public:
    QGridLayout *gridLayout;
    QGroupBox *GroupBox1;
    QGridLayout *gridLayout1;
    QGridLayout *gridLayout2;
    QGridLayout *gridLayout3;
    QGridLayout *gridLayout4;
    QPushButton *buttonRedo;
    QPushButton *buttonUndo;
    QSpacerItem *spacerItem;
    QPushButton *buttonApply;
    QGridLayout *gridLayout5;
    QSpinBox *SpinBox3;
    QSpinBox *SpinBox2;
    QLabel *TextLabel2;
    QSpacerItem *spacerItem1;
    QSpinBox *SpinBox4;
    QLabel *TextLabel3;
    QSpinBox *SpinBox1;
    QLabel *TextLabel1;
    QLabel *TextLabel4;

    void setupUi(QWidget *EntityGUI_4Spin_QTD)
    {
    EntityGUI_4Spin_QTD->setObjectName(QString::fromUtf8("EntityGUI_4Spin_QTD"));
    gridLayout = new QGridLayout(EntityGUI_4Spin_QTD);
    gridLayout->setSpacing(6);
    gridLayout->setMargin(0);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    GroupBox1 = new QGroupBox(EntityGUI_4Spin_QTD);
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

    spacerItem = new QSpacerItem(0, 45, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout3->addItem(spacerItem, 2, 0, 1, 1);

    buttonApply = new QPushButton(GroupBox1);
    buttonApply->setObjectName(QString::fromUtf8("buttonApply"));

    gridLayout3->addWidget(buttonApply, 0, 0, 1, 1);


    gridLayout2->addLayout(gridLayout3, 0, 1, 1, 1);

    gridLayout5 = new QGridLayout();
    gridLayout5->setSpacing(6);
    gridLayout5->setMargin(0);
    gridLayout5->setObjectName(QString::fromUtf8("gridLayout5"));
    SpinBox3 = new QSpinBox(GroupBox1);
    SpinBox3->setObjectName(QString::fromUtf8("SpinBox3"));
    QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(7), static_cast<QSizePolicy::Policy>(0));
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(SpinBox3->sizePolicy().hasHeightForWidth());
    SpinBox3->setSizePolicy(sizePolicy);

    gridLayout5->addWidget(SpinBox3, 2, 1, 1, 1);

    SpinBox2 = new QSpinBox(GroupBox1);
    SpinBox2->setObjectName(QString::fromUtf8("SpinBox2"));
    QSizePolicy sizePolicy1(static_cast<QSizePolicy::Policy>(7), static_cast<QSizePolicy::Policy>(0));
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(SpinBox2->sizePolicy().hasHeightForWidth());
    SpinBox2->setSizePolicy(sizePolicy1);

    gridLayout5->addWidget(SpinBox2, 1, 1, 1, 1);

    TextLabel2 = new QLabel(GroupBox1);
    TextLabel2->setObjectName(QString::fromUtf8("TextLabel2"));
    QSizePolicy sizePolicy2(static_cast<QSizePolicy::Policy>(0), static_cast<QSizePolicy::Policy>(0));
    sizePolicy2.setHorizontalStretch(0);
    sizePolicy2.setVerticalStretch(0);
    sizePolicy2.setHeightForWidth(TextLabel2->sizePolicy().hasHeightForWidth());
    TextLabel2->setSizePolicy(sizePolicy2);
    TextLabel2->setWordWrap(false);

    gridLayout5->addWidget(TextLabel2, 1, 0, 1, 1);

    spacerItem1 = new QSpacerItem(0, 70, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout5->addItem(spacerItem1, 4, 1, 1, 1);

    SpinBox4 = new QSpinBox(GroupBox1);
    SpinBox4->setObjectName(QString::fromUtf8("SpinBox4"));
    QSizePolicy sizePolicy3(static_cast<QSizePolicy::Policy>(7), static_cast<QSizePolicy::Policy>(0));
    sizePolicy3.setHorizontalStretch(0);
    sizePolicy3.setVerticalStretch(0);
    sizePolicy3.setHeightForWidth(SpinBox4->sizePolicy().hasHeightForWidth());
    SpinBox4->setSizePolicy(sizePolicy3);

    gridLayout5->addWidget(SpinBox4, 3, 1, 1, 1);

    TextLabel3 = new QLabel(GroupBox1);
    TextLabel3->setObjectName(QString::fromUtf8("TextLabel3"));
    QSizePolicy sizePolicy4(static_cast<QSizePolicy::Policy>(0), static_cast<QSizePolicy::Policy>(0));
    sizePolicy4.setHorizontalStretch(0);
    sizePolicy4.setVerticalStretch(0);
    sizePolicy4.setHeightForWidth(TextLabel3->sizePolicy().hasHeightForWidth());
    TextLabel3->setSizePolicy(sizePolicy4);
    TextLabel3->setWordWrap(false);

    gridLayout5->addWidget(TextLabel3, 2, 0, 1, 1);

    SpinBox1 = new QSpinBox(GroupBox1);
    SpinBox1->setObjectName(QString::fromUtf8("SpinBox1"));
    QSizePolicy sizePolicy5(static_cast<QSizePolicy::Policy>(7), static_cast<QSizePolicy::Policy>(0));
    sizePolicy5.setHorizontalStretch(0);
    sizePolicy5.setVerticalStretch(0);
    sizePolicy5.setHeightForWidth(SpinBox1->sizePolicy().hasHeightForWidth());
    SpinBox1->setSizePolicy(sizePolicy5);

    gridLayout5->addWidget(SpinBox1, 0, 1, 1, 1);

    TextLabel1 = new QLabel(GroupBox1);
    TextLabel1->setObjectName(QString::fromUtf8("TextLabel1"));
    QSizePolicy sizePolicy6(static_cast<QSizePolicy::Policy>(0), static_cast<QSizePolicy::Policy>(0));
    sizePolicy6.setHorizontalStretch(0);
    sizePolicy6.setVerticalStretch(0);
    sizePolicy6.setHeightForWidth(TextLabel1->sizePolicy().hasHeightForWidth());
    TextLabel1->setSizePolicy(sizePolicy6);
    TextLabel1->setWordWrap(false);

    gridLayout5->addWidget(TextLabel1, 0, 0, 1, 1);

    TextLabel4 = new QLabel(GroupBox1);
    TextLabel4->setObjectName(QString::fromUtf8("TextLabel4"));
    QSizePolicy sizePolicy7(static_cast<QSizePolicy::Policy>(0), static_cast<QSizePolicy::Policy>(0));
    sizePolicy7.setHorizontalStretch(0);
    sizePolicy7.setVerticalStretch(0);
    sizePolicy7.setHeightForWidth(TextLabel4->sizePolicy().hasHeightForWidth());
    TextLabel4->setSizePolicy(sizePolicy7);
    TextLabel4->setWordWrap(false);

    gridLayout5->addWidget(TextLabel4, 3, 0, 1, 1);


    gridLayout2->addLayout(gridLayout5, 0, 0, 1, 1);


    gridLayout1->addLayout(gridLayout2, 0, 0, 1, 1);


    gridLayout->addWidget(GroupBox1, 0, 0, 1, 1);


    retranslateUi(EntityGUI_4Spin_QTD);

    QSize size(255, 154);
    size = size.expandedTo(EntityGUI_4Spin_QTD->minimumSizeHint());
    EntityGUI_4Spin_QTD->resize(size);


    QMetaObject::connectSlotsByName(EntityGUI_4Spin_QTD);
    } // setupUi

    void retranslateUi(QWidget *EntityGUI_4Spin_QTD)
    {
    EntityGUI_4Spin_QTD->setWindowTitle(QApplication::translate("EntityGUI_4Spin_QTD", "EntityGUI_4Spin_QTD", 0, QApplication::UnicodeUTF8));
    GroupBox1->setTitle(QApplication::translate("EntityGUI_4Spin_QTD", "Values", 0, QApplication::UnicodeUTF8));
    buttonRedo->setText(QApplication::translate("EntityGUI_4Spin_QTD", "Redo", 0, QApplication::UnicodeUTF8));
    buttonUndo->setText(QApplication::translate("EntityGUI_4Spin_QTD", "Undo", 0, QApplication::UnicodeUTF8));
    buttonApply->setText(QApplication::translate("EntityGUI_4Spin_QTD", "Create", 0, QApplication::UnicodeUTF8));
    TextLabel2->setText(QApplication::translate("EntityGUI_4Spin_QTD", "TL2", 0, QApplication::UnicodeUTF8));
    TextLabel3->setText(QApplication::translate("EntityGUI_4Spin_QTD", "TL3", 0, QApplication::UnicodeUTF8));
    TextLabel1->setText(QApplication::translate("EntityGUI_4Spin_QTD", "TL1", 0, QApplication::UnicodeUTF8));
    TextLabel4->setText(QApplication::translate("EntityGUI_4Spin_QTD", "TL4", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(EntityGUI_4Spin_QTD);
    } // retranslateUi

};

namespace Ui {
    class EntityGUI_4Spin_QTD: public Ui_EntityGUI_4Spin_QTD {};
} // namespace Ui

#endif // ENTITYGUI_4SPIN_QTD_H

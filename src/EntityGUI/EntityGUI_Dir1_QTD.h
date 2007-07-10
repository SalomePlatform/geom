/********************************************************************************
** Form generated from reading ui file 'EntityGUI_Dir1_QTD.ui'
**
** Created: Tue Jul 10 10:20:45 2007
**      by: Qt User Interface Compiler version 4.2.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef ENTITYGUI_DIR1_QTD_H
#define ENTITYGUI_DIR1_QTD_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QRadioButton>
#include <QtGui/QWidget>
#include <Qt3Support/Q3MimeSourceFactory>

class Ui_EntityGUI_Dir1_QTD
{
public:
    QGridLayout *gridLayout;
    QGroupBox *GroupDir1;
    QGridLayout *gridLayout1;
    QGridLayout *gridLayout2;
    QRadioButton *RB_Dir12;
    QRadioButton *RB_Dir13;
    QRadioButton *RB_Dir11;
    QRadioButton *RB_Dir14;

    void setupUi(QWidget *EntityGUI_Dir1_QTD)
    {
    EntityGUI_Dir1_QTD->setObjectName(QString::fromUtf8("EntityGUI_Dir1_QTD"));
    gridLayout = new QGridLayout(EntityGUI_Dir1_QTD);
    gridLayout->setSpacing(6);
    gridLayout->setMargin(0);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    GroupDir1 = new QGroupBox(EntityGUI_Dir1_QTD);
    GroupDir1->setObjectName(QString::fromUtf8("GroupDir1"));
    QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(7), static_cast<QSizePolicy::Policy>(0));
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(GroupDir1->sizePolicy().hasHeightForWidth());
    GroupDir1->setSizePolicy(sizePolicy);
    gridLayout1 = new QGridLayout(GroupDir1);
    gridLayout1->setSpacing(6);
    gridLayout1->setMargin(11);
    gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
    gridLayout2 = new QGridLayout();
    gridLayout2->setSpacing(6);
    gridLayout2->setMargin(0);
    gridLayout2->setObjectName(QString::fromUtf8("gridLayout2"));
    RB_Dir12 = new QRadioButton(GroupDir1);
    RB_Dir12->setObjectName(QString::fromUtf8("RB_Dir12"));

    gridLayout2->addWidget(RB_Dir12, 1, 0, 1, 1);

    RB_Dir13 = new QRadioButton(GroupDir1);
    RB_Dir13->setObjectName(QString::fromUtf8("RB_Dir13"));

    gridLayout2->addWidget(RB_Dir13, 2, 0, 1, 1);

    RB_Dir11 = new QRadioButton(GroupDir1);
    RB_Dir11->setObjectName(QString::fromUtf8("RB_Dir11"));

    gridLayout2->addWidget(RB_Dir11, 0, 0, 1, 1);

    RB_Dir14 = new QRadioButton(GroupDir1);
    RB_Dir14->setObjectName(QString::fromUtf8("RB_Dir14"));

    gridLayout2->addWidget(RB_Dir14, 3, 0, 1, 1);


    gridLayout1->addLayout(gridLayout2, 0, 0, 1, 1);


    gridLayout->addWidget(GroupDir1, 0, 0, 1, 1);

    QWidget::setTabOrder(RB_Dir11, RB_Dir12);
    QWidget::setTabOrder(RB_Dir12, RB_Dir13);
    QWidget::setTabOrder(RB_Dir13, RB_Dir14);

    retranslateUi(EntityGUI_Dir1_QTD);

    QSize size(131, 123);
    size = size.expandedTo(EntityGUI_Dir1_QTD->minimumSizeHint());
    EntityGUI_Dir1_QTD->resize(size);


    QMetaObject::connectSlotsByName(EntityGUI_Dir1_QTD);
    } // setupUi

    void retranslateUi(QWidget *EntityGUI_Dir1_QTD)
    {
    EntityGUI_Dir1_QTD->setWindowTitle(QApplication::translate("EntityGUI_Dir1_QTD", "EntityGUI_Dir1_QTD", 0, QApplication::UnicodeUTF8));
    GroupDir1->setTitle(QApplication::translate("EntityGUI_Dir1_QTD", "Direction", 0, QApplication::UnicodeUTF8));
    RB_Dir12->setText(QApplication::translate("EntityGUI_Dir1_QTD", "Perpendicular", 0, QApplication::UnicodeUTF8));
    RB_Dir13->setText(QApplication::translate("EntityGUI_Dir1_QTD", "Tangent", 0, QApplication::UnicodeUTF8));
    RB_Dir11->setText(QApplication::translate("EntityGUI_Dir1_QTD", "Angle", 0, QApplication::UnicodeUTF8));
    RB_Dir14->setText(QApplication::translate("EntityGUI_Dir1_QTD", "VX-VY", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(EntityGUI_Dir1_QTD);
    } // retranslateUi

};

namespace Ui {
    class EntityGUI_Dir1_QTD: public Ui_EntityGUI_Dir1_QTD {};
} // namespace Ui

#endif // ENTITYGUI_DIR1_QTD_H

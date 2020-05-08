/* This was based upon a small Qt5 example. Copyright is still the same, but i'm also a contributor to this. */

/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "window.h"

#include <iostream>

#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

char text[25][256] =
{
	{"Background number"},
	{"Special actions"},
	{"1st character : 1st Sprite on-screen"},
	{"1st character : 2nd Sprite on-screen"},
	{"1st charac : Last frame"},
	{"1st charac : Last frame 2nd Sprite (nonlooped)"},
	{"Time needed until last frame"},
	{"Size in width"},
	{"Size in height"},
	{"2nd character : 1st Sprite on-screen"},
	{"2nd character : 2nd Sprite on-screen"},
	{"2nd charac : Last frame"},
	{"2nd charac : Last frame 2nd Sprite (nonlooped)"},
	{"Time needed until last frame"},
	{"Size in width"},
	{"Size in height"},
	{"Choice ?"},
	{"Teleport position array for Choice 1"},
	{"Teleport position array for Choice 2"},
	{"Music to play"},
	{"Sound effect to play"},
	{"Character voice to play"},
	{"Text (line 1, If a character speaks, type the name of character with :)"},
	{"Text (line 2)"},
	{"Text (line 3)"}
};

char placeholder[25][256] =
{
	{"-1 for pitch black"},
	{"See documentation for numbers (Unused)"},
	
	{"Set sprite number. 0 to disable it."},
	{"0 to disable the non looping sprite that is used before the 1st sprite"},
	{"Last frame number in the spritesheet for sprite 1"},
	{"Last frame number in the spritesheet for sprite 2 (non looped)"},
	{"100 is equal to one second"},
	{"From 0 to 100. (Relative to the screen's resoluton)"},
	{"From 0 to 100. (Relative to the screen's resoluton)"},
	
	{"Set sprite number. 0 to disable it."},
	{"0 to disable the non looping sprite that is used before the 1st sprite"},
	{"Last frame number in the spritesheet for sprite 1"},
	{"Last frame number in the spritesheet for sprite 2 (non looped)"},
	{"100 is equal to one second"},
	{"From 0 to 100. (Relative to the screen's resoluton)"},
	{"From 0 to 100. (Relative to the screen's resoluton)"},
	
	{"Set to 1 if there's a choice, 0 to disable."},
	{"Position inside of the array for choice 1."},
	{"Position inside of the array for choice 2."},
	
	{"Music slot to play. -1 to ignore, -2 for silence."},
	{"Sound effect slot to play. -1 to ignore"},
	{"Voice to play, -1 to ignore"},
	
	{"Line 1 for Text."},
	{"Line 2 for Text"},
	{"Line 3 for Text"}
};


Window::Window(QWidget *parent) : QWidget(parent)
{
	int i;
	QGroupBox *ob_group[32];
	QGridLayout *ob_layout[32];
	QGridLayout *layout;
	
	setWindowTitle(tr("VN Editor"));
	
	layout = new QGridLayout;
	
	for(i=0;i<2;i++)
	{
		ob_group[i] = new QGroupBox(tr(text[i]));
		ob_edit[i] = new QLineEdit;
		ob_edit[i]->setPlaceholderText(placeholder[i]);
		ob_edit[i]->setFocus();

		ob_layout[i] = new QGridLayout;
		ob_layout[i]->addWidget(ob_edit[i], 1, 0, 1, 2);
		ob_group[i]->setLayout(ob_layout[i]);
		
		layout->addWidget(ob_group[i], i, 0);
	}
	
	for(i=2;i<9;i++)
	{
		ob_group[i] = new QGroupBox(tr(text[i]));
		ob_edit[i] = new QLineEdit;
		ob_edit[i]->setPlaceholderText(placeholder[i]);
		ob_edit[i]->setFocus();

		ob_layout[i] = new QGridLayout;
		ob_layout[i]->addWidget(ob_edit[i], 1, 0, 1, 2);
		ob_group[i]->setLayout(ob_layout[i]);
		
		layout->addWidget(ob_group[i], i-2, 1);
	}
	
	for(i=9;i<16;i++)
	{
		ob_group[i] = new QGroupBox(tr(text[i]));
		ob_edit[i] = new QLineEdit;
		ob_edit[i]->setPlaceholderText(placeholder[i]);
		ob_edit[i]->setFocus();

		ob_layout[i] = new QGridLayout;
		ob_layout[i]->addWidget(ob_edit[i], 1, 0, 1, 2);
		ob_group[i]->setLayout(ob_layout[i]);
		
		layout->addWidget(ob_group[i], i-9, 2);
	}
	
	for(i=16;i<19;i++)
	{
		ob_group[i] = new QGroupBox(tr(text[i]));
		ob_edit[i] = new QLineEdit;
		ob_edit[i]->setPlaceholderText(placeholder[i]);
		ob_edit[i]->setFocus();

		ob_layout[i] = new QGridLayout;
		ob_layout[i]->addWidget(ob_edit[i], 1, 0, 1, 2);
		ob_group[i]->setLayout(ob_layout[i]);
		
		layout->addWidget(ob_group[i], i-16, 3);
	}
	
	for(i=19;i<22;i++)
	{
		ob_group[i] = new QGroupBox(tr(text[i]));
		ob_edit[i] = new QLineEdit;
		ob_edit[i]->setPlaceholderText(placeholder[i]);
		ob_edit[i]->setFocus();

		ob_layout[i] = new QGridLayout;
		ob_layout[i]->addWidget(ob_edit[i], 1, 0, 1, 2);
		ob_group[i]->setLayout(ob_layout[i]);
		
		layout->addWidget(ob_group[i], i-19, 4);
	}
	
	for(i=22;i<25;i++)
	{
		ob_group[i] = new QGroupBox(tr(text[i]));
		ob_edit[i] = new QLineEdit;
		ob_edit[i]->setPlaceholderText(placeholder[i]);
		ob_edit[i]->setFocus();

		ob_layout[i] = new QGridLayout;
		ob_layout[i]->addWidget(ob_edit[i], 1, 0, 1, 2);
		ob_group[i]->setLayout(ob_layout[i]);
		
		layout->addWidget(ob_group[i], i-22, 5);
	}
	
	m_button = new QPushButton;
	m_button->setText("Output text (output.txt)");
	m_button->resize(150, 25);
	m_button->setFont(QFont("Droid Sans Mono", 12, QFont::Normal));
	layout->addWidget(m_button, 13, 2);
    // Connect button signal to appropriate slot
    connect(m_button, SIGNAL (released()), this, SLOT (handleButton()));
	
	setLayout(layout);
}


void Window::handleButton()
{
	uint32_t i;
	QByteArray array;
	char *buffer;
	
	FILE* fp;
	fp = fopen("output.txt", "w");
	
	for(i=0;i<22;i++)
	{
		array = ob_edit[i]->text().toLocal8Bit();
		buffer = array.data();
		fprintf(fp, "%s|", buffer);
	}
	
	for(i=22;i<25;i++)
	{
		array = ob_edit[i]->text().toLocal8Bit();
		buffer = array.data();
		if (i != 24) fprintf(fp, "%s\\n", buffer);
		else fprintf(fp, "%s\n", buffer);
	}
	
	fprintf(fp, "\n");
	fclose(fp);
}

/*
This file is part of Telegram Desktop,
the official desktop version of Telegram messaging app, see https://telegram.org

Telegram Desktop is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

It is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

In addition, as a special exception, the copyright holders give permission
to link the code of portions of this program with the OpenSSL library.

Full license: https://github.com/telegramdesktop/tdesktop/blob/master/LICENSE
Copyright (c) 2014-2015 John Preston, https://desktop.telegram.org
*/
#pragma once

#include "gui/twidget.h"
#include "gui/boxshadow.h"

class Dropdown : public TWidget {
	Q_OBJECT

public:

	Dropdown(QWidget *parent, const style::dropdown &st = st::dropdownDef);

	IconedButton *addButton(IconedButton *button);
	void resetButtons();
	void updateButtons();

	void resizeEvent(QResizeEvent *e);
	void paintEvent(QPaintEvent *e);

	void enterEvent(QEvent *e);
	void leaveEvent(QEvent *e);
	void keyPressEvent(QKeyEvent *e);
	void otherEnter();
	void otherLeave();

	void fastHide();
	void ignoreShow(bool ignore = true);

	void step_appearance(float64 ms, bool timer);

	bool eventFilter(QObject *obj, QEvent *e);

	bool overlaps(const QRect &globalRect) {
		if (isHidden() || _a_appearance.animating()) return false;

		return QRect(_st.padding.left(),
					 _st.padding.top(),
					 _width - _st.padding.left() - _st.padding.right(),
					 _height - _st.padding.top() - _st.padding.bottom()
					 ).contains(QRect(mapFromGlobal(globalRect.topLeft()), globalRect.size()));
	}

signals:

	void hiding();

public slots:

	void hideStart();
	void hideFinish();

	void showStart();
	void onWndActiveChanged();

	void buttonStateChanged(int oldState, ButtonStateChangeSource source);

private:

	void adjustButtons();

	bool _ignore;

	typedef QVector<IconedButton*> Buttons;
	Buttons _buttons;

	int32 _selected;

	const style::dropdown &_st;

	int32 _width, _height;
	bool _hiding;

	anim::fvalue a_opacity;
	Animation _a_appearance;

	QTimer _hideTimer;

	BoxShadow _shadow;

};

class DragArea : public TWidget {
	Q_OBJECT

public:

	DragArea(QWidget *parent);

	void paintEvent(QPaintEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void dragEnterEvent(QDragEnterEvent *e);
	void dragLeaveEvent(QDragLeaveEvent *e);
	void dropEvent(QDropEvent *e);
	void dragMoveEvent(QDragMoveEvent *e);

	void setText(const QString &text, const QString &subtext);

	void otherEnter();
	void otherLeave();

	void fastHide();

	void step_appearance(float64 ms, bool timer);

	bool overlaps(const QRect &globalRect) {
		if (isHidden() || _a_appearance.animating()) return false;

		return QRect(st::dragPadding.left(),
					 st::dragPadding.top(),
					 width() - st::dragPadding.left() - st::dragPadding.right(),
					 height() - st::dragPadding.top() - st::dragPadding.bottom()
					 ).contains(QRect(mapFromGlobal(globalRect.topLeft()), globalRect.size()));
	}

signals:

	void dropped(const QMimeData *data);

public slots:

	void hideStart();
	void hideFinish();

	void showStart();

private:

	bool _hiding, _in;

	anim::fvalue a_opacity;
	anim::cvalue a_color;
	Animation _a_appearance;

	BoxShadow _shadow;

	QString _text, _subtext;

};

class EmojiPanel;
static const int EmojiColorsCount = 5;

class EmojiColorPicker : public TWidget {
	Q_OBJECT

public:

	EmojiColorPicker();

	void showEmoji(uint32 code);

	void paintEvent(QPaintEvent *e);
	void enterEvent(QEvent *e);
	void leaveEvent(QEvent *e);
	void mousePressEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);

	void step_appearance(float64 ms, bool timer);
	void step_selected(uint64 ms, bool timer);
	void showStart();

	void clearSelection(bool fast = false);

public slots:

	void hideStart(bool fast = false);

signals:

	void emojiSelected(EmojiPtr emoji);
	void hidden();

private:

	void drawVariant(Painter &p, int variant);

	void updateSelected();

	bool _ignoreShow;

	EmojiPtr _variants[EmojiColorsCount + 1];

	typedef QMap<int32, uint64> EmojiAnimations; // index - showing, -index - hiding
	EmojiAnimations _emojiAnimations;
	Animation _a_selected;

	float64 _hovers[EmojiColorsCount + 1];

	int32 _selected, _pressedSel;
	QPoint _lastMousePos;

	bool _hiding;
	QPixmap _cache;

	anim::fvalue a_opacity;
	Animation _a_appearance;

	QTimer _hideTimer;

	BoxShadow _shadow;

};

class EmojiPanInner : public TWidget {
	Q_OBJECT

public:

	EmojiPanInner();

	void setMaxHeight(int32 h);
	void paintEvent(QPaintEvent *e);

	void mousePressEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void leaveEvent(QEvent *e);
	void leaveToChildEvent(QEvent *e);
	void enterFromChildEvent(QEvent *e);

	void step_selected(uint64 ms, bool timer);
	void hideFinish();

	void showEmojiPack(DBIEmojiTab packIndex);

	void clearSelection(bool fast = false);

	DBIEmojiTab currentTab(int yOffset) const;

	void refreshRecent();

	void setScrollTop(int top);

	void fillPanels(QVector<EmojiPanel*> &panels);
	void refreshPanels(QVector<EmojiPanel*> &panels);
	
public slots:

	void updateSelected();

	void onShowPicker();
	void onPickerHidden();
	void onColorSelected(EmojiPtr emoji);

	bool checkPickerHide();

signals:

	void selected(EmojiPtr emoji);

	void switchToStickers();

	void scrollToY(int y);
	void disableScroll(bool dis);

	void needRefreshPanels();
	void saveConfigDelayed(int32 delay);

private:

	int32 _maxHeight;

	int32 countHeight();
	void selectEmoji(EmojiPtr emoji);

	QRect emojiRect(int tab, int sel);

	typedef QMap<int32, uint64> Animations; // index - showing, -index - hiding
	Animations _animations;
	Animation _a_selected;

	int32 _top, _counts[emojiTabCount];

	QVector<EmojiPtr> _emojis[emojiTabCount];
	QVector<float64> _hovers[emojiTabCount];

	int32 _esize;

	int32 _selected, _pressedSel, _pickerSel;
	QPoint _lastMousePos;

	EmojiColorPicker _picker;
	QTimer _showPickerTimer;
};

struct StickerIcon {
	StickerIcon(uint64 setId) : setId(setId), sticker(0), pixw(0), pixh(0) {
	}
	StickerIcon(uint64 setId, DocumentData *sticker, int32 pixw, int32 pixh) : setId(setId), sticker(sticker), pixw(pixw), pixh(pixh) {
	}
	uint64 setId;
	DocumentData *sticker;
	int32 pixw, pixh;
};

struct ContextResult {
	ContextResult(uint64 queryId)
		: queryId(queryId)
		, doc(0)
		, photo(0)
		, width(0)
		, height(0)
		, duration(0)
		, noWebPage(false) {
	}
	uint64 queryId;
	QString id, type;
	DocumentData *doc;
	PhotoData *photo;
	QString title, description, url, thumb_url;
	QString content_type, content_url;
	int32 width, height, duration;

	QString message; // botContextMessageText
	bool noWebPage;
	EntitiesInText entities;
	QString caption; // if message.isEmpty() use botContextMessageMediaAuto
};
typedef QList<ContextResult*> ContextResults;

class StickerPanInner : public TWidget {
	Q_OBJECT

public:

	StickerPanInner();

	void setMaxHeight(int32 h);
	void paintEvent(QPaintEvent *e);

	void mousePressEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void leaveEvent(QEvent *e);
	void leaveToChildEvent(QEvent *e);
	void enterFromChildEvent(QEvent *e);

	void step_selected(uint64 ms, bool timer);

	void hideFinish();
	void showStickerSet(uint64 setId);

	void clearSelection(bool fast = false);

	void refreshStickers();
	void refreshRecentStickers(bool resize = true);
	void refreshSavedGifs();
	void refreshContextRows(const ContextResults &results);
	void refreshRecent();

	void fillIcons(QList<StickerIcon> &icons);
	void fillPanels(QVector<EmojiPanel*> &panels);
	void refreshPanels(QVector<EmojiPanel*> &panels);

	void setScrollTop(int top);
	void preloadImages();

	uint64 currentSet(int yOffset) const;
	void refreshContextResults(const ContextResults &results);
	void contextBotChanged();

	void ui_repaintContextItem(const LayoutContextItem *layout);
	bool ui_isContextItemVisible(const LayoutContextItem *layout);
	bool ui_isContextItemBeingChosen();

	~StickerPanInner() {
		clearContextRows();
		deleteUnusedGifLayouts();
		deleteUnusedContextLayouts();
	}

public slots:

	void updateSelected();
	void onSettings();
	void onPreview();

signals:

	void selected(DocumentData *sticker);
	void removing(quint64 setId);

	void refreshIcons();

	void switchToEmoji();

	void scrollToY(int y);
	void scrollUpdated();
	void disableScroll(bool dis);
	void needRefreshPanels();

	void saveConfigDelayed(int32 delay);

private:

	void paintContextItems(Painter &p, const QRect &r);
	void paintStickers(Painter &p, const QRect &r);
		
	int32 _maxHeight;

	void appendSet(uint64 setId);

	int32 countHeight();
	void selectEmoji(EmojiPtr emoji);
	QRect stickerRect(int tab, int sel);

	typedef QMap<int32, uint64> Animations; // index - showing, -index - hiding
	Animations _animations;
	Animation _a_selected;

	int32 _top;

	struct DisplayedSet {
		DisplayedSet(uint64 id, int32 flags, const QString &title, int32 hoversSize, const StickerPack &pack = StickerPack()) : id(id), flags(flags), title(title), hovers(hoversSize, 0), pack(pack) {
		}
		uint64 id;
		int32 flags;
		QString title;
		QVector<float64> hovers;
		StickerPack pack;
	};
	QList<DisplayedSet> _sets;
	QList<bool> _custom;

	bool _showingSavedGifs, _showingContextItems;

	typedef QList<LayoutContextItem*> ContextItems;
	struct ContextRow {
		ContextRow() : height(0) {
		}
		int32 height;
		ContextItems items;
	};
	typedef QList<ContextRow> ContextRows;
	ContextRows _contextRows;
	void clearContextRows();

	typedef QMap<DocumentData*, LayoutContextGif*> GifLayouts;
	GifLayouts _gifLayouts;
	LayoutContextGif *layoutPrepare(DocumentData *doc, int32 position, int32 width);

	typedef QMap<ContextResult*, LayoutContextItem*> ContextLayouts;
	ContextLayouts _contextLayouts;

	ContextRow &layoutContextRow(ContextRow &row, int32 *widths, int32 sumWidth);
	void deleteUnusedGifLayouts();

	void deleteUnusedContextLayouts();

	int32 _selected, _pressedSel;
	QPoint _lastMousePos;
	TextLinkPtr _linkOver, _linkDown;

	LinkButton _settings;

	QTimer _previewTimer;
	bool _previewShown;
};

class EmojiPanel : public TWidget {
	Q_OBJECT

public:

	EmojiPanel(QWidget *parent, const QString &text, uint64 setId, bool special, int32 wantedY); // NoneStickerSetId if in emoji
	void setText(const QString &text);
	void setDeleteVisible(bool isVisible);

	void paintEvent(QPaintEvent *e);
	void mousePressEvent(QMouseEvent *e);

	int32 wantedY() const {
		return _wantedY;
	}
	void setWantedY(int32 y) {
		_wantedY = y;
	}

signals:

	void deleteClicked(quint64 setId);
	void mousePressed();

public slots:

	void onDelete();

private:

	void updateText();

	int32 _wantedY;
	QString _text, _fullText;
	uint64 _setId;
	bool _special, _deleteVisible;
	IconedButton *_delete;

};

class EmojiSwitchButton : public Button {
public:

	EmojiSwitchButton(QWidget *parent, bool toStickers); // otherwise toEmoji
	void paintEvent(QPaintEvent *e);
	void updateText();

protected:

	bool _toStickers;
	QString _text;
	int32 _textWidth;

};

class EmojiPan : public TWidget, public RPCSender {
	Q_OBJECT

public:

	EmojiPan(QWidget *parent);

	void setMaxHeight(int32 h);
	void paintEvent(QPaintEvent *e);

	void enterEvent(QEvent *e);
	void leaveEvent(QEvent *e);
	void otherEnter();
	void otherLeave();

	void mousePressEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);

	bool event(QEvent *e);

	void fastHide();
	bool hiding() const {
		return _hiding || _hideTimer.isActive();
	}

	void step_appearance(float64 ms, bool timer);
	void step_slide(float64 ms, bool timer);
	void step_icons(uint64 ms, bool timer);

	bool eventFilter(QObject *obj, QEvent *e);
	void stickersInstalled(uint64 setId);

	void showContextResults(UserData *bot, QString query);
	void contextBotChanged();

	bool overlaps(const QRect &globalRect) {
		if (isHidden() || !_cache.isNull()) return false;

		return QRect(st::dropdownDef.padding.left(),
					 st::dropdownDef.padding.top(),
					 _width - st::dropdownDef.padding.left() - st::dropdownDef.padding.right(),
					 _height - st::dropdownDef.padding.top() - st::dropdownDef.padding.bottom()
					 ).contains(QRect(mapFromGlobal(globalRect.topLeft()), globalRect.size()));
	}

	void ui_repaintContextItem(const LayoutContextItem *layout);
	bool ui_isContextItemVisible(const LayoutContextItem *layout);
	bool ui_isContextItemBeingChosen();

public slots:

	void refreshStickers();
	void refreshSavedGifs();

	void hideStart();
	void hideFinish();

	void showStart();
	void onWndActiveChanged();

	void onTabChange();
	void onScroll();
	void onSwitch();

	void onRemoveSet(quint64 setId);
	void onRemoveSetSure();
	void onDelayedHide();

	void onRefreshIcons();
	void onRefreshPanels();

	void onSaveConfig();
	void onSaveConfigDelayed(int32 delay);

	void onContextRequest();

signals:

	void emojiSelected(EmojiPtr emoji);
	void stickerSelected(DocumentData *sticker);
	void updateStickers();

private:

	void validateSelectedIcon(bool animated = false);

	int32 _maxHeight;
	bool _horizontal;

	void leaveToChildEvent(QEvent *e);

	void updateSelected();
	void updateIcons();

	void prepareTab(int32 &left, int32 top, int32 _width, FlatRadiobutton &tab);
	void updatePanelsPositions(const QVector<EmojiPanel*> &panels, int32 st);

	void showAll();
	void hideAll();

	bool _noTabUpdate;

	int32 _width, _height;
	bool _hiding;
	QPixmap _cache;

	anim::fvalue a_opacity;
	Animation _a_appearance;

	QTimer _hideTimer;

	BoxShadow _shadow;

	FlatRadiobutton _recent, _people, _nature, _food, _activity, _travel, _objects, _symbols;
	QList<StickerIcon> _icons;
	QVector<float64> _iconHovers;
	int32 _iconOver, _iconSel, _iconDown;
	bool _iconsDragging;
	typedef QMap<int32, uint64> Animations; // index - showing, -index - hiding
	Animations _iconAnimations;
	Animation _a_icons;
	QPoint _iconsMousePos, _iconsMouseDown;
	int32 _iconsLeft, _iconsTop;
	int32 _iconsStartX, _iconsMax;
	anim::ivalue _iconsX, _iconSelX;
	uint64 _iconsStartAnim;

	bool _stickersShown;
	QPixmap _fromCache, _toCache;
	anim::ivalue a_fromCoord, a_toCoord;
	anim::fvalue a_fromAlpha, a_toAlpha;
	Animation _a_slide;

	ScrollArea e_scroll;
	EmojiPanInner e_inner;
	QVector<EmojiPanel*> e_panels;
	EmojiSwitchButton e_switch;
	ScrollArea s_scroll;
	StickerPanInner s_inner;
	QVector<EmojiPanel*> s_panels;
	EmojiSwitchButton s_switch;

	uint64 _removingSetId;

	QTimer _saveConfigTimer;

	// context bots
	struct ContextCacheEntry {
		~ContextCacheEntry() {
			clearResults();
		}
		QString nextOffset;
		ContextResults results;
		void clearResults() {
			for (int32 i = 0, l = results.size(); i < l; ++i) {
				delete results.at(i);
			}
			results.clear();
		}
	};
	typedef QMap<QString, ContextCacheEntry*> ContextCache;
	ContextCache _contextCache;
	QTimer _contextRequestTimer;

	void refreshContextRows(bool newResults);
	UserData *_contextBot;
	QString _contextQuery, _contextNextQuery, _contextNextOffset;
	mtpRequestId _contextRequestId;
	void contextResultsDone(const MTPmessages_BotResults &result);
	bool contextResultsFail(const RPCError &error);

};

typedef QList<UserData*> MentionRows;
typedef QList<QString> HashtagRows;
typedef QList<QPair<UserData*, const BotCommand*> > BotCommandRows;

class MentionsDropdown;
class MentionsInner : public TWidget {
	Q_OBJECT

public:

	MentionsInner(MentionsDropdown *parent, MentionRows *mrows, HashtagRows *hrows, BotCommandRows *brows);

	void paintEvent(QPaintEvent *e);

	void enterEvent(QEvent *e);
	void leaveEvent(QEvent *e);

	void mousePressEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);

	void clearSel();
	bool moveSel(int direction);
	bool select();

	QString getSelected() const;

signals:

	void chosen(QString mentionOrHashtag);
	void mustScrollTo(int scrollToTop, int scrollToBottom);

public slots:

	void onParentGeometryChanged();
	void onUpdateSelected(bool force = false);

private:

	void setSel(int sel, bool scroll = false);

	MentionsDropdown *_parent;
	MentionRows *_mrows;
	HashtagRows *_hrows;
	BotCommandRows *_brows;
	int32 _sel;
	bool _mouseSel;
	QPoint _mousePos;

	bool _overDelete;
};

class MentionsDropdown : public TWidget {
	Q_OBJECT

public:

	MentionsDropdown(QWidget *parent);

	void paintEvent(QPaintEvent *e);

	void fastHide();

	bool clearFilteredBotCommands();
	void showFiltered(PeerData *peer, QString start);
	void updateFiltered(bool toDown = false);
	void setBoundings(QRect boundings);

	void step_appearance(float64 ms, bool timer);

	const QString &filter() const;
	ChatData *chat() const;
	ChannelData *channel() const;
	UserData *user() const;

	int32 innerTop();
	int32 innerBottom();

	bool eventFilter(QObject *obj, QEvent *e);
	QString getSelected() const;

	bool overlaps(const QRect &globalRect) {
		if (isHidden() || !testAttribute(Qt::WA_OpaquePaintEvent)) return false;

		return rect().contains(QRect(mapFromGlobal(globalRect.topLeft()), globalRect.size()));
	}

	~MentionsDropdown();

signals:

	void chosen(QString mentionOrHashtag);

public slots:

	void hideStart();
	void hideFinish();

	void showStart();

private:

	void recount(bool toDown = false);

	QPixmap _cache;
	MentionRows _mrows;
	HashtagRows _hrows;
	BotCommandRows _brows;

	void rowsUpdated(const MentionRows &rows, const HashtagRows &hrows, const BotCommandRows &brows, bool toDown);

	ScrollArea _scroll;
	MentionsInner _inner;

	ChatData *_chat;
	UserData *_user;
	ChannelData *_channel;
	QString _filter;
	QRect _boundings;

	int32 _width, _height;
	bool _hiding;

	anim::fvalue a_opacity;
	Animation _a_appearance;

	QTimer _hideTimer;

	BoxShadow _shadow;

};

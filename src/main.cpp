#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/GJAccountManager.hpp>

using namespace geode::prelude;

bool isBackingUp = false;

class DumbPopup final : public Popup<>, GJAccountBackupDelegate {
protected:
	void onClose(CCObject* sender) override {
		if (isBackingUp) return Notification::create("Slow down there! I'm still backing up.")->show();
		Popup::onClose(sender);
	}
	bool setup() override {
		m_noElasticity = true;
		this->setTitle("Control Panel"_spr);
		CCMenuItemSpriteExtra* backupButton = CCMenuItemSpriteExtra::create(
			ButtonSprite::create("Backup", 1.f),
			this, menu_selector(DumbPopup::onBackup)
		);
		CCMenuItemSpriteExtra* closeButton = CCMenuItemSpriteExtra::create(
			ButtonSprite::create("Close Game", 1.f),
			this, menu_selector(DumbPopup::onCloseGame)
		);
		m_buttonMenu->addChild(backupButton);
		backupButton->setPosition(128, 100);
		m_buttonMenu->addChild(closeButton);
		closeButton->setPosition(128, 50);
		return true;
	}
public:
	void onBackup(CCObject* sender) {
		if (isBackingUp) return Notification::create("Slow down there! I'm still backing up.")->show();
		auto gjam = GJAccountManager::get();
		gjam->m_backupDelegate = this;
		bool result = gjam->getAccountBackupURL();
		isBackingUp = true;
		Notification::create(fmt::format("Backing up now... (Status: {})", result))->show();
	}
	void onCloseGame(CCObject* sender) {
		if (isBackingUp) return Notification::create("Slow down there! I'm still backing up.")->show();
		if (MenuLayer* menuLayer = CCScene::get()->getChildByType<MenuLayer>(0)) menuLayer->endGame();
	}
	virtual void backupAccountFinished() {
		log::info("backupAccountFinished called");
		GJAccountManager::get()->m_backupDelegate = nullptr;
		Notification::create("Backup finished. It succeeded.")->show();
		isBackingUp = false;
	}
    virtual void backupAccountFailed(BackupAccountError p0, int p1) {
		log::info("backupAccountFailed called");
		GJAccountManager::get()->m_backupDelegate = nullptr;
		Notification::create(fmt::format("Backup finished. It failed. (p0: {}, p1: {})", static_cast<int>(p0), p1))->show();
		isBackingUp = false;
	}
	static DumbPopup* create() {
		auto ret = new DumbPopup();
		const CCSize ws = CCDirector::get()->getWinSize() / 2.f;
		if (ret && ret->initAnchored(256, 160)) {
			ret->autorelease();
			return ret;
		}
		delete ret;
		return nullptr;
	}
};

class $modify(MyMenuLayer, MenuLayer) {
	bool init() {
		if (!MenuLayer::init()) return false;
		auto bottomMenu = getChildByID("bottom-menu");
		if (!bottomMenu) return true;
		bottomMenu->addChild(
			CCMenuItemSpriteExtra::create(
				CircleButtonSprite::createWithSpriteFrameName(
					"pixelitem_139_001.png", .75f, CircleBaseColor::Green,
					CircleBaseSize::MediumAlt
				), this, menu_selector(MyMenuLayer::onDumbPopup)
			)
		);
		bottomMenu->updateLayout();
		return true;
	}
	void onDumbPopup(CCObject*) {
		if (DumbPopup* dp = DumbPopup::create(); dp) dp->show();
	}
};

class $modify(MyGJAccountManager, GJAccountManager) {
	bool getAccountBackupURL() {
		bool result = GJAccountManager::getAccountBackupURL();
		if (isBackingUp) {
			log::info("==================================");
			log::info("got account backup URL: {}", result);
		}
		return result;
	}
	void onGetAccountBackupURLCompleted(gd::string response, gd::string tag) {
		GJAccountManager::onGetAccountBackupURLCompleted(response, tag);
		if (isBackingUp) {
			log::info("----------------------------------");
			log::info("response: {}", response);
			log::info("tag: {}", tag);
		}
	}
	bool backupAccount(gd::string url) {
		bool result = GJAccountManager::backupAccount(url);
		if (isBackingUp) {
			log::info("----------------------------------");
			log::info("url: {}", url);
		}
		return result;
	}
	void ProcessHttpRequest(gd::string endpoint, gd::string params, gd::string tag, GJHttpType httpType) {
		GJAccountManager::ProcessHttpRequest(endpoint, params, tag, httpType);
		if (isBackingUp) {
			log::info("----------------------------------");
			log::info("endpoint: {}", endpoint);
			log::info("params: {}", params);
			log::info("tag: {}", tag);
			log::info("httpType as int: {}", static_cast<int>(httpType));
		}
	}
	void onProcessHttpRequestCompleted(cocos2d::extension::CCHttpClient* client, cocos2d::extension::CCHttpResponse* response) {
		GJAccountManager::onProcessHttpRequestCompleted(client, response);
		if (isBackingUp) {
			log::info("----------------------------------");
			log::info("client: {}", client);
			log::info("response: {}", response);
		}
	}
	void handleIt(bool success, gd::string response, gd::string tag, GJHttpType type) {
		GJAccountManager::handleIt(success, response, tag, type);
		if (isBackingUp) {
			log::info("----------------------------------");
			log::info("success: {}", success);
			log::info("response: {}", response);
			log::info("tag: {}", tag);
			log::info("httpType as int: {}", static_cast<int>(httpType));
		}
	}
	void onBackupAccountCompleted(gd::string response, gd::string tag) {
		GJAccountManager::onBackupAccountCompleted(response, tag);
		if (isBackingUp) {
			log::info("----------------------------------");
			log::info("response: {}", response);
			log::info("tag: {}", tag);
		}
	}
};
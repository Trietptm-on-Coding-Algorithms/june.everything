﻿// Decompiled by AS3 Sorcerer 3.00
// http://www.as3sorcerer.com/

//------------------------------------------------------------
//jxjy_AS3_V01_01_fla.MainTimeline

package jxjy_AS3_V01_01_fla
{
    import flash.display.MovieClip;
    import flash.text.TextField;
    import flash.display.SimpleButton;
    import flash.utils.Timer;
    import flash.external.ExternalInterface;
    import flash.display.StageDisplayState;
    import flash.events.MouseEvent;
    import flash.display.Loader;
    import flash.events.Event;
    import flash.events.ProgressEvent;
    import flash.events.IOErrorEvent;
    import flash.net.URLRequest;
    import flash.net.URLLoader;
    import flash.ui.ContextMenu;
    import flash.ui.ContextMenuItem;
    import flash.events.ContextMenuEvent;
    import flash.net.navigateToURL;
    import flash.display.LoaderInfo;
    import flash.events.TimerEvent;
    import flash.system.Security;
    import flash.display.*;
    import flash.events.*;
    import flash.filters.*;
    import flash.text.*;
    import flash.ui.*;
    import flash.geom.*;
    import flash.utils.*;
    import flash.external.*;
    import adobe.utils.*;
    import flash.accessibility.*;
    import flash.errors.*;
    import flash.media.*;
    import flash.net.*;
    import flash.printing.*;
    import flash.profiler.*;
    import flash.sampler.*;
    import flash.system.*;
    import flash.xml.*;

    public dynamic class MainTimeline extends MovieClip 
    {

        public var tTips:TextField;
        public var btn_screenSwap:SimpleButton;
        public var btn_pageNext:SimpleButton;
        public var tfMsg:TextField;
        public var mc_slidebarContent:MovieClip;
        public var btn_saveTime:SimpleButton;
        public var mc_loading:MovieClip;
        public var mc_soundControler:MovieClip;
        public var mc_noInit:MovieClip;
        public var btn_test:MovieClip;
        public var txt_pageNum:TextField;
        public var btn_pagePrev:SimpleButton;
        public var mc_contnetControler:MovieClip;
        public var mc_movieBox:MovieClip;
        public var STATUS_NOMOVIE:String;		// 没有视频
        public var STATUS_LOADING:String;		// 加载
        public var STATUS_PLAYING:String;		// 播放
        public var STATUS_PAUSED:String;		// 暂停
        public var STATUS_TESTING:String;		// 测试（视频学习完成后弹出测试题供学员做）
        public var STATUS_CHECKING:String;		// 检查（检查学员的答题）
        public var STATUS_PLAYDONE:String;		// 播放完毕
        public var TEST_DEFAULT_FIRST_TIME:Number;
        public var TEST_DEFAULT_INTERVAL_TIME:Number;
        public var SAVE_BUTTON:String;
        public var SAVE_PAGE:String;
        public var SAVE_JAVASCRIPT:String;
        public var mCfgMaxTime:Number;
        public var mCfgMaxTimeMultiple:Number;
        public var mCfgRightMenuName:String;
        public var mCfgRightMenuUrl:String;
        public var curPage:Number;
        public var pages:Array;
        public var totalPage:int;
        public var strBasePath:String;
        public var arrCurProgress:Array;
        public var eduMallXml:XML;
        public var sysStatus:String;		// 系统状态
        public var isInstructions:Boolean;	// 是否指令
        public var timer:Timer;
        public var testTime:Number;
        public var bTest:Boolean;
        public var firstTestTime:Number;
        public var testTimeInterval:Number;
        public var oldStudyTime:Number;
        public var studyTime:Number;		// 学习时间
        public var pageTime:Number;
        public var maxTime:Number;
        public var clickCount:Number;//不论点击上一页，或者下一页，都会加1
        public var curMovie:MovieClip;

        public function MainTimeline()
        {
            addFrameScript(0, frame1);
        }

        public function callSaveTime(_arg_1:String):void
        {
            var _local_2:*;
            var _local_3:String;
            var _local_4:String;
            var _local_5:String;
            var _local_6:String;
            if (isInstructions)
            {
                return;
            };
            switch (sysStatus)
            {
                case STATUS_NOMOVIE:
                case STATUS_LOADING:
                case STATUS_TESTING:
                case STATUS_CHECKING:
                    return;
                case STATUS_PAUSED:
                case STATUS_PLAYING:
                case STATUS_PLAYDONE:	// 播放完成后触发的事件
                    _local_2 = studyTime;//将学习时间给变量2
                    if (_arg_1 == SAVE_BUTTON)
                    {
					// 如果用户点击的是保存按钮的话
					// 则学时就是当前学时加上页面时间，但这里似乎用不到的
                        _local_2 = (_local_2 + pageTime);	
                    };
                    if ((Math.round((_local_2 / 60)) - Math.round((oldStudyTime / 60))) < 1)
                    {//Math.round(x:Number) 返回一个与x接近的整数，例如：0.5 = 1 ，1.3 = 1 ，1.8 = 2
					// 
                        if (_arg_1 == SAVE_BUTTON)
                        {
                            Alert.show("学时保存成功。", "提示");
                        };
                        return;
                    };
                    oldStudyTime = _local_2;		// 然并卵，没什么大用啊
                    _local_3 = encrypt(_local_2);	// 这个重要，将学习时间加密
                    _local_4 = encrypt(clickCount);
                    _local_5 = getFixedLengthStr(("" + _local_4.length), 2, "0");
                    _local_6 = ((_local_5 + _local_3) + _local_4);
                    ExternalInterface.call("swfDo_Save", _local_6, createStrProgress(), _arg_1);
                    return;
                default:
                    Alert.show(("系统状态异常：" + sysStatus));
            };
        }

        public function createStrProgress():String
        {
            var _local_1 = "";
            _local_1 = (_local_1 + getFixedLengthStr(("" + arrCurProgress[0]), 2, "0"));
            _local_1 = (_local_1 + getFixedLengthStr(("" + arrCurProgress[1]), 2, "0"));
            _local_1 = (_local_1 + getFixedLengthStr(("" + arrCurProgress[2]), 2, "0"));
            return ((_local_1 + getFixedLengthStr(("" + arrCurProgress[3]), 3, "0")));
        }

        public function doTest():void
        {
            sysStatus = STATUS_TESTING;
            stage.displayState = StageDisplayState.NORMAL;
            setPageControlerStatus();
            btn_saveTime.enabled = false;
            btn_screenSwap.enabled = false;
            mc_slidebarContent.enabled = false;
            mc_slidebarContent.j_mc.enabled = false;
            mc_contnetControler.swapContentStatus();
            mc_soundControler.startTest();
            curMovie.stop();
            ExternalInterface.call("swfDo_test", 1);
        }

        public function encrypt(_arg_1:Number):String
        {
            var _local_2 = "ArsNDdeBChqOPSTtuvQREF67GH01KfgLIJwxyzM8iVWmXYjklnopUZc234ab59/=@]]]]";
            var _local_3:String = String(ExternalInterface.call("starttime"));// 读入开始学习时间 加密后的32位长度
			// 举例 _local_3 = "z0 m5 VM Q2 vN Nv ow md UH zJ UV QX W0 6f hC H3";
			// var str:String = "Hello from Paris, Texas!!!"; 
			// trace(str.substr(11,15)); // output: Paris, Texas!!! 
			// trace(str.substring(11,15)); // output: Pari
			// 在 substring() 方法中，第二个参数是子字符串的结尾 处字符的位置（该字符未包含在返回的字符串中）
            var _local_4:String = _local_3.substring(14, 15);	//m	从1开始计算
            var _local_5:String = _local_3.substring(25, 26);	//0
			// 搜索字符串，并返回在调用字符串内 startIndex 位置上或之后找到的 value 的第一个匹配项的位置。
			// 此索引从零开始，这意味着字符串中的第一个字符被视为位于索引 0 而不是索引 1 处。
			// 如果未找到 value，该方法会返回 -1。
            var _local_6:Number = _local_2.indexOf(_local_4);	// 43	// 'm' 在字符串中出现的位置 从0开始，不成功返回-1
            var _local_7:String = _local_2.substring((_local_6 + 1), (_local_6 + 2));
            var _local_8:Number = _local_2.indexOf(_local_5);
            var _local_9:String = _local_2.substring((_local_8 + 1), (_local_8 + 2));
            var _local_10:String = String((((_local_6 * 7) + (_local_8 * 3)) * _arg_1));
            var _local_11:String = (((_local_7 + _local_10.substring(0, 1)) + _local_9) + _local_10.substring(1, _local_10.length));
            return (_local_11);
        }

        public function getFixedLengthStr(_arg_1:String, _arg_2:Number, _arg_3:String):String
        {
            if (_arg_2 <= 0)
            {
                return ("");
            };
            if (_arg_2 <= _arg_1.length)
            {
                return (_arg_1.substr((_arg_1.length - _arg_2)));
            };
            if (_arg_3 == "")
            {
                _arg_3 = "0";
            };
            var _local_4 = "";
			// ceil（Math.ceil 方法）. public static ceil(x:Number) : Number. 
			// 返回指定数字或表达式的上限值。数字的上限值是大于等于该数字的最接近的整数。
			// ceil(12.5)=13
            var _local_5:Number = Math.ceil((_arg_2 / _arg_3.length));
            var _local_6:Number = 0;
            while (_local_6 < _local_5)
            {
                _local_4 = (_local_4 + _arg_3);
                _local_6++;
            };
            _arg_1 = (_local_4 + _arg_1);
            return (_arg_1.substr((_arg_1.length - _arg_2)));
        }

        public function init():void
        {
            btn_saveTime.visible = false;
            mc_loading.visible = false;
            txt_pageNum.text = "第0页/共0页";
            sysStatus = STATUS_NOMOVIE;
            isInstructions = false;
            stage.displayState = StageDisplayState.NORMAL;
            btn_pagePrev.addEventListener(MouseEvent.CLICK, onPagePrev);
            btn_pageNext.addEventListener(MouseEvent.CLICK, onPageNext);
            btn_saveTime.addEventListener(MouseEvent.CLICK, onSaveTime);
            btn_screenSwap.addEventListener(MouseEvent.CLICK, onSwapScreen);
            setPageControlerStatus();
            ExternalInterface.addCallback("replay", replay);
            ExternalInterface.addCallback("changeContent", changeContent);
            ExternalInterface.addCallback("faceCheckStart", faceCheckStart);
            ExternalInterface.addCallback("faceCheckDone", faceCheckDone);
            ExternalInterface.addCallback("setHasTest", setHasTest);
            ExternalInterface.addCallback("setFirstTime", setFirstTime);
            ExternalInterface.addCallback("setIntervalTime", setIntervalTime);
            ExternalInterface.addCallback("recoveWindow", recoveWindow);
            ExternalInterface.addCallback("asAlert", myAlert);
            var _local_1:String = this.loaderInfo.parameters["hasTest"];
            if (_local_1 == null)
            {
                bTest = true;
            } else
            {
                _local_1 = _local_1.toLowerCase();
                if (_local_1 == "n")
                {
                    bTest = false;
                } else
                {
                    bTest = true;
                };
            };
            firstTestTime = Number(this.loaderInfo.parameters["firstTime"]);
            if (isNaN(firstTestTime))
            {
                firstTestTime = TEST_DEFAULT_FIRST_TIME;
            } else
            {
                firstTestTime = Math.round(firstTestTime);
                if (firstTestTime <= 0)
                {
                    firstTestTime = TEST_DEFAULT_FIRST_TIME;
                };
            };
            testTimeInterval = Number(this.loaderInfo.parameters["intervalTime"]);
            if (isNaN(testTimeInterval))
            {
                testTimeInterval = TEST_DEFAULT_INTERVAL_TIME;
                firstTestTime = TEST_DEFAULT_FIRST_TIME;
            } else
            {
                testTimeInterval = Math.round(this.loaderInfo.parameters["testTimeInterval"]);
                if (testTimeInterval <= firstTestTime)
                {
                    testTimeInterval = TEST_DEFAULT_INTERVAL_TIME;
                    firstTestTime = TEST_DEFAULT_FIRST_TIME;
                };
            };
            tTips.text = "";
        }

        public function initCurProgress():void
        {
            var _local_2:String;
            var _local_1:Array = strBasePath.split("/");
            if (strBasePath.lastIndexOf("/") != (strBasePath.length - 1))
            {
                _local_2 = _local_1[(_local_1.length - 1)];
            } else
            {
                _local_2 = _local_1[(_local_1.length - 2)];
            };
            var _local_3:Array = _local_2.split("_");
            arrCurProgress.push(Number(_local_3[0]));
            arrCurProgress.push(Number(_local_3[1]));
            arrCurProgress.push(Number(_local_3[2]));
            arrCurProgress.push(curPage);
        }

        public function loadPage():void
        {
            sysStatus = STATUS_LOADING;
            mc_loading.visible = true;
            mc_loading.setPercent(0);
            setPageControlerStatus();
            if (curPage >= pages.length)
            {
                curPage = (pages.length - 1);
            };
            var _local_1:String = pages[curPage];
            var _local_2:Loader = new Loader();
            _local_2.contentLoaderInfo.addEventListener(Event.COMPLETE, onPageLoaded);
            _local_2.contentLoaderInfo.addEventListener(ProgressEvent.PROGRESS, onPageLoadProgress);
            _local_2.contentLoaderInfo.addEventListener(IOErrorEvent.IO_ERROR, onPageLoadError);
            pageTime = 0;
            var _local_3:URLRequest = new URLRequest((strBasePath + _local_1));
            _local_2.load(_local_3);
        }

        public function loadXml():void
        {
            testTime = 0;
            oldStudyTime = 0;
            studyTime = 0;
            pageTime = 0;
            maxTime = 0;
            clickCount = 0;
            arrCurProgress = new Array();
            initCurProgress();
            pages = new Array();
            var _local_1:URLRequest = new URLRequest((strBasePath + "eduMall.xml"));
            var _local_2:URLLoader = new URLLoader();
            _local_2.addEventListener(Event.COMPLETE, onXmlLoaded);
            _local_2.addEventListener(IOErrorEvent.IO_ERROR, onXmlIOError);
            _local_2.load(_local_1);
        }

        public function moviePlaydone():void
        {
            mc_contnetControler.swapContentStatus();
        }

        public function readConfig():void
        {
            var _local_1:XML = eduMallXml.child(0)[0];
            var _local_2:XML = _local_1.child(0)[0];
            mCfgMaxTime = Number(_local_2.attribute("value").toXMLString());
            var _local_3:XML = _local_1.child(1)[0];
            mCfgMaxTimeMultiple = Number(_local_3.attribute("value").toXMLString());
            var _local_4:XML = _local_1.child(2)[0];
            mCfgRightMenuName = _local_4.attribute("value").toXMLString();
            var _local_5:XML = _local_1.child(3)[0];
            mCfgRightMenuUrl = _local_5.attribute("value").toXMLString();
            this.contextMenu = createContextMenu();
        }

        public function readPages():void
        {
            var _local_3:Array;
            var _local_4:String;
            var _local_1:XMLList = eduMallXml.child(1)[0].child(0).children();
            totalPage = _local_1.length();
            var _local_2:int;
            while (_local_2 < totalPage)
            {
                _local_3 = new Array();
                _local_4 = _local_1[_local_2].attribute("file").toString();
                _local_3.push(_local_4);
                pages.push(_local_3);
                _local_2++;
            };
        }

        public function createContextMenu():ContextMenu
        {
            var _local_1:ContextMenu = new ContextMenu();
            _local_1.hideBuiltInItems();
            var _local_2:ContextMenuItem = new ContextMenuItem(mCfgRightMenuName);
            _local_2.addEventListener(ContextMenuEvent.MENU_ITEM_SELECT, goCompanyWebsite);
            _local_1.customItems.push(_local_2);
            return (_local_1);
        }

        public function goCompanyWebsite(_arg_1:ContextMenuEvent):void
        {
            var _local_2:URLRequest = new URLRequest(mCfgRightMenuUrl);
            navigateToURL(_local_2, "_blank");
        }

        public function setPageControlerStatus():void
        {
            switch (sysStatus)
            {
                case STATUS_NOMOVIE:
                case STATUS_LOADING:
                case STATUS_TESTING:
                case STATUS_CHECKING:
                    btn_pagePrev.enabled = false;
                    btn_pageNext.enabled = false;
                    return;
                case STATUS_PAUSED:
                case STATUS_PLAYING:
                case STATUS_PLAYDONE:
            };
            if (pages.length <= 1)
            {
                btn_pagePrev.enabled = false;
                btn_pageNext.enabled = false;
            } else
            {
                if (curPage <= 0)
                {
                    btn_pagePrev.enabled = false;
                } else
                {
                    btn_pagePrev.enabled = true;
                };
                if (curPage >= (pages.length - 1))
                {
                    btn_pageNext.enabled = false;
                } else
                {
                    btn_pageNext.enabled = true;
                };
            };
        }

        public function showPageNum():void
        {
            txt_pageNum.text = (((("第" + (curPage + 1)) + "页/共") + pages.length) + "页");
        }

        public function goCompanyWebsit(_arg_1:Object, _arg_2:ContextMenuItem):void
        {
            var _local_3:URLRequest = new URLRequest(mCfgRightMenuUrl);
            var _local_4 = "_blank";
            navigateToURL(_local_3, _local_4);
        }

        public function onPageLoaded(_arg_1:Event):void
        {
            var _local_2:LoaderInfo = LoaderInfo(_arg_1.currentTarget);
            var _local_3:MovieClip = MovieClip(_local_2.content);
            if (sysStatus != STATUS_CHECKING)
            {
                sysStatus = STATUS_PLAYING;
            };
            mc_loading.visible = false;
            stage.frameRate = _local_2.frameRate;
            maxTime = Math.floor((_local_3.mv_mc.totalFrames / _local_2.frameRate));
            timer = new Timer(1000);
            timer.addEventListener(TimerEvent.TIMER, onTimer);
            timer.start();
            curMovie = _local_3.mv_mc;
            mc_contnetControler.setMovieclip(curMovie);
            mc_slidebarContent.setMovieClip(curMovie);
            mc_movieBox.changeMovie(curMovie);
            showPageNum();
            arrCurProgress[3] = curPage;
            setPageControlerStatus();
            if (pages[curPage] == "end.swf")
            {
                tTips.text = "本教学内容已经播放完毕，请更换内容继续学习。";
            } else
            {
                tTips.text = (("本页完全播放最大可获得学时" + maxTime) + "秒，完全播放后，翻页才可以记录该学时。");
            };
        }

        public function onPageLoadError(_arg_1:IOErrorEvent):void
        {
            Alert.show(((("载入教学内容" + (strBasePath + pages[curPage])) + "时发生错误。\n错误详情：") + _arg_1.toString()));
            sysStatus = STATUS_NOMOVIE;
        }

        public function onPageLoadProgress(_arg_1:ProgressEvent):void
        {
            mc_loading.setPercent((_arg_1.bytesLoaded / _arg_1.bytesTotal));
        }

        public function onPageNext(_arg_1:MouseEvent):void
        {
            switch (sysStatus)
            {
                case STATUS_NOMOVIE:
                case STATUS_LOADING:
                case STATUS_TESTING:
                case STATUS_CHECKING:
                    return;
                case STATUS_PAUSED:
                case STATUS_PLAYING:
                case STATUS_PLAYDONE:
            };
            if (isNaN(curPage))
            {
                return;
            };
            if (curPage >= (pages.length - 1))
            {
                Alert.show("已经是最后一页！");
                return;
            };
            curPage++;
            clickCount++;
            setPageControlerStatus();
            timer.stop();
            studyTime = (studyTime + pageTime);
            pageTime = 0;
            callSaveTime(SAVE_PAGE);
            loadPage();
        }

        public function onPagePrev(_arg_1:Event):void
        {
            switch (sysStatus)
            {
                case STATUS_NOMOVIE:
                case STATUS_LOADING:
                case STATUS_TESTING:
                case STATUS_CHECKING:
                    return;
                case STATUS_PAUSED:
                case STATUS_PLAYING:
                case STATUS_PLAYDONE:
            };
            if (isNaN(curPage))
            {
                return;
            };
            if (curPage <= 0)
            {
                Alert.show("已经是第一页！");
                return;
            };
            curPage--;
            clickCount++;
            setPageControlerStatus();
            timer.stop();
            studyTime = (studyTime + pageTime);
            pageTime = 0;
            callSaveTime(SAVE_PAGE);
            loadPage();
        }

        public function onSaveTime(_arg_1:MouseEvent):void
        {
            callSaveTime(SAVE_BUTTON);
        }

        public function onSwapScreen(_arg_1:MouseEvent):void
        {
            if (stage.displayState == StageDisplayState.NORMAL)
            {
                stage.displayState = StageDisplayState.FULL_SCREEN;
            } else
            {
                stage.displayState = StageDisplayState.NORMAL;
            };
        }

        public function onTimer(_arg_1:TimerEvent):void
        {
            if (isInstructions)
            {
                return;
            };
            if (sysStatus != STATUS_PLAYING)
            {
                return;
            };
            pageTime++;
            if (pageTime >= maxTime)
            {
                timer.stop();
            };
            if (bTest)
            {
                testTime++;
                testTime = (testTime % testTimeInterval);
                if (testTime == firstTestTime)
                {
                    doTest();
                };
            };
        }

        public function onXmlLoaded(_arg_1:Event):void
        {
            var _local_2:URLLoader = URLLoader(_arg_1.target);
            eduMallXml = new XML(_local_2.data);
            readConfig();
            readPages();
            loadPage();
            setPageControlerStatus();
            mc_noInit.visible = false;
        }

        public function onXmlIOError(_arg_1:Event):void
        {
        }

        public function changeContent(_arg_1:String, _arg_2:String):void
        {
            switch (sysStatus)
            {
                case STATUS_LOADING:
                    Alert.show("当前有一个载入过程正在进行，请稍后重试。\n如果长时间不能完成载入，请刷新页面后，更换教学内容。");
                    return;
                case STATUS_TESTING:
                    Alert.show("请完成测试。");
                    return;
                case STATUS_CHECKING:
                    Alert.show("请完成人脸识别验证。");
                    return;
                case STATUS_PLAYING:
                case STATUS_PAUSED:
                case STATUS_PLAYDONE:
                case STATUS_NOMOVIE:
                    strBasePath = _arg_1;
                    if (strBasePath.charAt((strBasePath.length - 1)) != "/")
                    {
                        strBasePath = (strBasePath + "/");
                    };
                    curPage = (Number(_arg_2) - 1);
                    if (isNaN(curPage))
                    {
                        curPage = 0;
                    } else
                    {
                        if (curPage < 0)
                        {
                            curPage = 0;
                        };
                    };
                    sysStatus = STATUS_LOADING;
                    mc_contnetControler.swapContentStatus();
                    setPageControlerStatus();
                    loadXml();
                    return;
            };
        }

        public function faceCheckDone():void
        {
            sysStatus = STATUS_PLAYING;
            setPageControlerStatus();
            btn_saveTime.enabled = true;
            btn_screenSwap.enabled = true;
            mc_slidebarContent.enabled = true;
            mc_slidebarContent.j_mc.enabled = true;
            mc_contnetControler.swapContentStatus();
            mc_soundControler.endTest();
            curMovie.play();
        }

        public function faceCheckStart():void
        {
            sysStatus = STATUS_CHECKING;
            setPageControlerStatus();
            btn_saveTime.enabled = false;
            btn_screenSwap.enabled = false;
            mc_slidebarContent.enabled = false;
            mc_slidebarContent.j_mc.enabled = false;
            mc_contnetControler.swapContentStatus();
            mc_soundControler.startTest();
            curMovie.stop();
        }

        public function recoveWindow():void
        {
            stage.displayState = StageDisplayState.NORMAL;
        }

        public function replay():void
        {
            sysStatus = STATUS_PLAYING;
            setPageControlerStatus();
            btn_saveTime.enabled = true;
            btn_screenSwap.enabled = true;
            mc_slidebarContent.enabled = true;
            mc_slidebarContent.j_mc.enabled = true;
            mc_contnetControler.swapContentStatus();
            mc_soundControler.endTest();
            curMovie.play();
        }

        public function setHasTest(_arg_1:String):void
        {
            if (_arg_1.toLowerCase() == "n")
            {
                bTest = false;
            } else
            {
                bTest = true;
            };
        }

        public function setFirstTime(_arg_1:Number):void
        {
            firstTestTime = _arg_1;
            if (testTimeInterval <= firstTestTime)
            {
                testTimeInterval = TEST_DEFAULT_INTERVAL_TIME;
                firstTestTime = TEST_DEFAULT_FIRST_TIME;
            };
        }

        public function setIntervalTime(_arg_1:Number):void
        {
            testTimeInterval = _arg_1;
            if (testTimeInterval <= firstTestTime)
            {
                testTimeInterval = TEST_DEFAULT_INTERVAL_TIME;
                firstTestTime = TEST_DEFAULT_FIRST_TIME;
            };
        }

        public function myAlert(_arg_1:String):void
        {
            Alert.show(("来自JS：" + _arg_1));
        }

        public function onTest(_arg_1:MouseEvent):void
        {
        }

        function frame1()
        {
            STATUS_NOMOVIE = "nomovie";
            STATUS_LOADING = "loading";
            STATUS_PLAYING = "playing";
            STATUS_PAUSED = "paused";
            STATUS_TESTING = "testing";
            STATUS_CHECKING = "checking";
            STATUS_PLAYDONE = "playdone";
            TEST_DEFAULT_FIRST_TIME = 14;
            TEST_DEFAULT_INTERVAL_TIME = 180;
            SAVE_BUTTON = "button";
            SAVE_PAGE = "page";
            SAVE_JAVASCRIPT = "javascript";
            mCfgMaxTime = 0;
            mCfgMaxTimeMultiple = 0;
            mCfgRightMenuName = "";
            mCfgRightMenuUrl = "";
            totalPage = 0;
            stop();
            Security.allowDomain("*");
            Security.allowInsecureDomain("*");
            Alert.init(stage);
            init();
            btn_test.addEventListener(MouseEvent.CLICK, onTest);
        }


    }
}//package jxjy_AS3_V01_01_fla

//------------------------------------------------------------
//jxjy_AS3_V01_01_fla.Mc_ContentControler_6

package jxjy_AS3_V01_01_fla
{
    import flash.display.MovieClip;
    import flash.display.SimpleButton;
    import flash.events.MouseEvent;
    import flash.display.*;
    import flash.events.*;
    import flash.filters.*;
    import flash.text.*;
    import flash.ui.*;
    import flash.geom.*;
    import flash.utils.*;
    import flash.external.*;
    import adobe.utils.*;
    import flash.accessibility.*;
    import flash.errors.*;
    import flash.media.*;
    import flash.net.*;
    import flash.printing.*;
    import flash.profiler.*;
    import flash.sampler.*;
    import flash.system.*;
    import flash.xml.*;

    public dynamic class Mc_ContentControler_6 extends MovieClip 
    {

        public var btn_contentPlay:SimpleButton;
        public var btn_contentPause:SimpleButton;
        public var ctrledMc:MovieClip;

        public function Mc_ContentControler_6()
        {
            addFrameScript(0, frame1);
        }

        public function onBtnPlayClick(_arg_1:MouseEvent):void
        {
            if ((((Object(root).sysStatus == Object(root).STATUS_PAUSED)) || ((Object(root).sysStatus == Object(root).STATUS_PLAYDONE))))
            {
                Object(root).sysStatus = Object(root).STATUS_PLAYING;
                ctrledMc.play();
                swapContentStatus();
            };
        }

        public function onBtnPauseClick(_arg_1:MouseEvent):void
        {
            if (Object(root).sysStatus == Object(root).STATUS_PLAYING)
            {
                Object(root).sysStatus = Object(root).STATUS_PAUSED;
                ctrledMc.stop();
                swapContentStatus();
            };
        }

        public function setMovieclip(_arg_1:MovieClip):void
        {
            ctrledMc = _arg_1;
            if (ctrledMc == null)
            {
                btn_contentPlay.enabled = false;
                btn_contentPause.enabled = false;
                btn_contentPlay.visible = true;
                btn_contentPause.visible = false;
            } else
            {
                btn_contentPlay.enabled = true;
                btn_contentPause.enabled = true;
                btn_contentPlay.visible = false;
                btn_contentPause.visible = true;
                if (Object(root).sysStatus != Object(root).STATUS_CHECKING)
                {
                    ctrledMc.play();
                } else
                {
                    ctrledMc.stop();
                };
            };
        }

        public function swapContentStatus():void
        {
            switch (Object(root).sysStatus)
            {
                case Object(root).STATUS_NOMOVIE:
                case Object(root).STATUS_LOADING:
                case Object(root).STATUS_TESTING:
                case Object(root).STATUS_CHECKING:
                    btn_contentPlay.visible = true;
                    btn_contentPause.visible = false;
                    btn_contentPlay.enabled = false;
                    btn_contentPause.enabled = false;
                    return;
                case Object(root).STATUS_PLAYING:
                    btn_contentPlay.visible = false;
                    btn_contentPause.visible = true;
                    btn_contentPlay.enabled = true;
                    btn_contentPause.enabled = true;
                    return;
                case Object(root).STATUS_PAUSED:
                    btn_contentPlay.visible = true;
                    btn_contentPause.visible = false;
                    btn_contentPlay.enabled = true;
                    btn_contentPause.enabled = true;
                    return;
                case Object(root).STATUS_PLAYDONE:
                    btn_contentPlay.visible = true;
                    btn_contentPause.visible = false;
                    btn_contentPlay.enabled = true;
                    btn_contentPause.enabled = true;
                    return;
                default:
                    btn_contentPlay.visible = true;
                    btn_contentPause.visible = false;
                    btn_contentPlay.enabled = false;
                    btn_contentPause.enabled = false;
            };
        }

        function frame1()
        {
            stop();
            ctrledMc = null;
            btn_contentPlay.enabled = false;
            btn_contentPause.enabled = false;
            btn_contentPlay.visible = true;
            btn_contentPause.visible = false;
            btn_contentPlay.addEventListener(MouseEvent.CLICK, onBtnPlayClick);
            btn_contentPause.addEventListener(MouseEvent.CLICK, onBtnPauseClick);
        }


    }
}//package jxjy_AS3_V01_01_fla

//------------------------------------------------------------
//jxjy_AS3_V01_01_fla.Mc_MovieBox_5

package jxjy_AS3_V01_01_fla
{
    import flash.display.MovieClip;

    public dynamic class Mc_MovieBox_5 extends MovieClip 
    {

        public var curMc:MovieClip;

        public function Mc_MovieBox_5()
        {
            addFrameScript(0, frame1);
        }

        public function changeMovie(_arg_1:MovieClip):void
        {
            if (curMc != null)
            {
                curMc.stop();
                this.removeChild(curMc);
            };
            this.addChild(_arg_1);
            curMc = _arg_1;
        }

        public function playMovie():void
        {
            if (curMc != null)
            {
                curMc.play();
            };
        }

        public function pauseMovie():void
        {
            if (curMc != null)
            {
                curMc.stop();
            };
        }

        function frame1()
        {
            curMc = null;
        }


    }
}//package jxjy_AS3_V01_01_fla

//------------------------------------------------------------
//jxjy_AS3_V01_01_fla.Mc_ProgressBarLoading_16

package jxjy_AS3_V01_01_fla
{
    import flash.display.MovieClip;
    import flash.text.TextField;

    public dynamic class Mc_ProgressBarLoading_16 extends MovieClip 
    {

        public var txt_percent:TextField;

        public function Mc_ProgressBarLoading_16()
        {
            addFrameScript(0, frame1);
        }

        public function setPercent(_arg_1:Number):void
        {
            var _local_2:String = ("" + (_arg_1 * 100));
            var _local_3:Number = _local_2.indexOf(".");
            if (_local_3 == -1)
            {
                txt_percent.text = (_local_2 + ".00%");
            } else
            {
                _local_2 = (_local_2 + "00");
                _local_2 = _local_2.substr(0, (_local_3 + 3));
                txt_percent.text = (_local_2 + "%");
            };
        }

        function frame1()
        {
            txt_percent.text = "0.00%";
        }


    }
}//package jxjy_AS3_V01_01_fla

//------------------------------------------------------------
//jxjy_AS3_V01_01_fla.Mc_SlideBarContent_21

package jxjy_AS3_V01_01_fla
{
    import flash.display.MovieClip;
    import flash.text.TextField;
    import flash.events.MouseEvent;
    import flash.events.Event;
    import flash.display.*;
    import flash.events.*;
    import flash.filters.*;
    import flash.text.*;
    import flash.ui.*;
    import flash.geom.*;
    import flash.utils.*;
    import flash.external.*;
    import adobe.utils.*;
    import flash.accessibility.*;
    import flash.errors.*;
    import flash.media.*;
    import flash.net.*;
    import flash.printing.*;
    import flash.profiler.*;
    import flash.sampler.*;
    import flash.system.*;
    import flash.xml.*;

    public dynamic class Mc_SlideBarContent_21 extends MovieClip 
    {

        public var c_txt:TextField;
        public var mc_mask:MovieClip;
        public var j_mc:MovieClip;
        public var bg_mc:MovieClip;
        public var myMc:MovieClip;
        public var totalSecond:String;
        public var totalFrame:Number;
        public var w:Number;
        public var left:Number;

        public function Mc_SlideBarContent_21()
        {
            addFrameScript(0, frame1);
        }

        public function setMovieClip(_arg_1:MovieClip):void
        {
            myMc = _arg_1;
            if (myMc == null)
            {
                removeEventListener(MouseEvent.CLICK, gotoProgress);
                j_mc.removeEventListener(MouseEvent.CLICK, enterFrameAction);
                Object(root).sysStatus = Object(root).STATUS_NOMOVIE;
            } else
            {
                totalFrame = myMc.totalFrames;
                totalSecond = ("" + Object(root).maxTime);
                this.addEventListener(MouseEvent.CLICK, gotoProgress);
                myMc.addEventListener(Event.ENTER_FRAME, enterFrameAction);
            };
        }

        public function enterFrameAction(_arg_1:Event):void
        {
            switch (Object(root).sysStatus)
            {
                case Object(root).STATUS_NOMOVIE:
                case Object(root).STATUS_LOADING:
                case Object(root).STATUS_TESTING:
                case Object(root).STATUS_CHECKING:
                case Object(root).STATUS_PLAYDONE:
                case Object(root).STATUS_PAUSED:
                    return;
                case Object(root).STATUS_PLAYING:
                    showProgress();
                    return;
            };
        }

        public function gotoProgress(_arg_1:MouseEvent):void
        {
            var _local_2:Object = _arg_1.target;
            if (((!((_local_2 == j_mc))) && (!((_local_2 == mc_mask)))))
            {
                return;
            };
            switch (Object(root).sysStatus)
            {
                case Object(root).STATUS_NOMOVIE:
                case Object(root).STATUS_LOADING:
                case Object(root).STATUS_TESTING:
                case Object(root).STATUS_CHECKING:
                    return;
                case Object(root).STATUS_PLAYDONE:
                case Object(root).STATUS_PAUSED:
                case Object(root).STATUS_PLAYING:
                    break;
            };
            var _local_3:Number = (((mouseX - left) / w) * totalFrame);
            if (Object(root).sysStatus == Object(root).STATUS_PLAYING)
            {
                myMc.gotoAndPlay(Math.floor(_local_3));
            } else
            {
                myMc.gotoAndStop(Math.floor(_local_3));
            };
            showProgress();
        }

        public function showProgress():void
        {
            var _local_1:Number = myMc.currentFrame;
            var _local_2:Number = ((w * _local_1) / totalFrame);
            mc_mask.x = (left + _local_2);
            mc_mask.width = (w - _local_2);
            var _local_3:String = ("" + (_local_1 / stage.frameRate));
            var _local_4:Number = _local_3.indexOf(".");
            if (_local_4 != -1)
            {
                _local_3 = _local_3.substr(0, _local_4);
            };
            c_txt.text = (((("第" + _local_3) + "秒/本页共") + totalSecond) + "秒");
            if (_local_1 == totalFrame)
            {
                Object(root).sysStatus = Object(root).STATUS_PLAYDONE;
                Object(root).moviePlaydone();
            };
        }

        function frame1()
        {
            c_txt.text = "第0秒/本页共0秒";
            myMc = null;
            w = mc_mask.width;
            left = mc_mask.getBounds(this).left;
        }


    }
}//package jxjy_AS3_V01_01_fla

//------------------------------------------------------------
//jxjy_AS3_V01_01_fla.Mc_SoundControler_11

package jxjy_AS3_V01_01_fla
{
    import flash.display.MovieClip;
    import flash.display.SimpleButton;
    import flash.events.MouseEvent;
    import flash.media.SoundMixer;
    import flash.media.SoundTransform;

    public dynamic class Mc_SoundControler_11 extends MovieClip 
    {

        public var btn_soundPlay:SimpleButton;
        public var btn_soundMute:SimpleButton;
        public var isMute:Boolean;

        public function Mc_SoundControler_11()
        {
            addFrameScript(0, frame1);
        }

        public function swapSoundStatus(_arg_1:MouseEvent):void
        {
            isMute = !(isMute);
            if (isMute)
            {
                btn_soundMute.visible = false;
                btn_soundPlay.visible = true;
                setVolume(0);
            } else
            {
                btn_soundMute.visible = true;
                btn_soundPlay.visible = false;
                setVolume(1);
            };
        }

        public function startTest():void
        {
            btn_soundMute.enabled = false;
            btn_soundPlay.enabled = false;
        }

        public function endTest():void
        {
            btn_soundMute.enabled = true;
            btn_soundPlay.enabled = true;
        }

        public function setVolume(_arg_1:Number):void
        {
            var _local_2:SoundTransform = SoundMixer.soundTransform;
            _local_2.volume = _arg_1;
            SoundMixer.soundTransform = _local_2;
        }

        function frame1()
        {
            isMute = false;
            btn_soundMute.visible = true;
            btn_soundPlay.visible = false;
            setVolume(1);
            btn_soundMute.addEventListener(MouseEvent.CLICK, swapSoundStatus);
            btn_soundPlay.addEventListener(MouseEvent.CLICK, swapSoundStatus);
        }


    }
}//package jxjy_AS3_V01_01_fla

//------------------------------------------------------------
//Alert

package 
{
    import flash.display.Stage;
    import flash.display.Sprite;
    import flash.display.SimpleButton;
    import flash.events.MouseEvent;
    import flash.display.BitmapData;
    import flash.geom.Rectangle;
    import flash.geom.Point;
    import flash.display.Bitmap;
    import flash.filters.BlurFilter;
    import flash.geom.Matrix;
    import flash.display.GradientType;
    import flash.filters.BitmapFilterQuality;
    import flash.filters.BitmapFilter;
    import flash.filters.DropShadowFilter;
    import flash.filters.GlowFilter;
    import flash.text.TextField;
    import flash.text.TextFieldAutoSize;
    import flash.text.TextFormat;
    import flash.text.TextFormatAlign;

    public class Alert 
    {

        private static var stage:Stage = null;
        private static var btnWidth:int = 75;
        private static var btnHeight:int = 18;
        private static var minimumWidths:Array = new Array(150, 230, 310);


        public static function init(_arg_1:Stage):void
        {
            stage = _arg_1;
        }

        public static function show(_arg_1:*="Made by dVyper", _arg_2:Object=null):void
        {
            if (stage == null)
            {
                trace("Alert class has not been initialised!");
                return;
            };
            var _local_3:AlertOptions = new AlertOptions(_arg_2, _arg_1);
            var _local_4:Sprite = new Sprite();
            _local_4.addChild(createBackground(_local_3));
            _local_4.addChild(getPrompt(_local_3));
            assignListeners(_local_4, _local_3);
            stage.addChild(_local_4);
        }

        private static function assignListeners(myAlert:Sprite, alertOptions:AlertOptions):void
        {
            var allButtons:Array;
            var n:int;
            var myFunction:Function;
            var button:SimpleButton;
            myFunction = function (_arg_1:MouseEvent):void
            {
                var _local_2:int;
                while (_local_2 < allButtons.length)
                {
                    allButtons[_local_2].removeEventListener(MouseEvent.CLICK, myFunction);
                    _local_2++;
                };
                closeAlert(myAlert);
                if (alertOptions.callback != null)
                {
                    alertOptions.callback(_arg_1.target.name);
                };
            };
            var promptBackground:* = myAlert.getChildAt(1);
            allButtons = new Array();
            while (n < alertOptions.buttons.length)
            {
                button = promptBackground.getChildByName(alertOptions.buttons[n]);
                button.addEventListener(MouseEvent.CLICK, myFunction);
                allButtons.push(button);
                n = (n + 1);
            };
        }

        private static function closeAlert(_arg_1:Sprite):void
        {
            var _local_2:* = _arg_1.getChildAt(1);
            _local_2.removeEventListener(MouseEvent.MOUSE_DOWN, doStartDrag);
            _local_2.removeEventListener(MouseEvent.MOUSE_UP, doStopDrag);
            stage.removeChild(_arg_1);
            _arg_1 = null;
        }

        private static function createBackground(_arg_1:AlertOptions):Sprite
        {
            var _local_3:int;
            var _local_4:BitmapData;
            var _local_5:BitmapData;
            var _local_6:Rectangle;
            var _local_7:Point;
            var _local_8:uint;
            var _local_9:Bitmap;
            var _local_2:Sprite = new Sprite();
            _local_3 = _arg_1.colour;
            switch (_arg_1.background)
            {
                case "blur":
                    _local_4 = new BitmapData(stage.stageWidth, stage.stageHeight, true, (0xFF000000 + _local_3));
                    _local_5 = new BitmapData(stage.stageWidth, stage.stageHeight);
                    _local_5.draw(stage);
                    _local_6 = new Rectangle(0, 0, stage.stageWidth, stage.stageHeight);
                    _local_7 = new Point(0, 0);
                    _local_8 = 120;
                    _local_4.merge(_local_5, _local_6, _local_7, _local_8, _local_8, _local_8, _local_8);
                    _local_4.applyFilter(_local_4, _local_6, _local_7, new BlurFilter(5, 5));
                    _local_9 = new Bitmap(_local_4);
                    _local_2.addChild(_local_9);
                    break;
                case "none":
                    _local_2.graphics.beginFill(_local_3, 0);
                    _local_2.graphics.drawRect(0, 0, stage.stageWidth, stage.stageHeight);
                    _local_2.graphics.endFill();
                    break;
                case "nonenotmodal":
                    break;
                case "simple":
                    _local_2.graphics.beginFill(_local_3, 0.3);
                    _local_2.graphics.drawRect(0, 0, stage.stageWidth, stage.stageHeight);
                    _local_2.graphics.endFill();
                    break;
            };
            return (_local_2);
        }

        private static function createButton(_arg_1:String, _arg_2:AlertOptions):SimpleButton
        {
            var _local_3:Array = new Array();
            var _local_4:Array = new Array(1, 1);
            var _local_5:Array = new Array(0, 0xFF);
            var _local_6:Matrix = new Matrix();
            _local_6.createGradientBox(btnWidth, btnHeight, (Math.PI / 2), 0, 0);
            var _local_7:int = 2;
            var _local_8:Sprite = new Sprite();
            _local_3 = [0xFFFFFF, _arg_2.colour];
            _local_8.graphics.lineStyle(3, brightenColour(_arg_2.colour, -50));
            _local_8.graphics.beginGradientFill(GradientType.LINEAR, _local_3, _local_4, _local_5, _local_6);
            _local_8.graphics.drawRoundRect(0, 0, btnWidth, btnHeight, _local_7, _local_7);
            _local_8.addChild(createButtonTextField(_arg_1, _arg_2));
            var _local_9:Sprite = new Sprite();
            _local_3 = [0xFFFFFF, brightenColour(_arg_2.colour, 50)];
            _local_9.graphics.lineStyle(1, brightenColour(_arg_2.colour, -50));
            _local_9.graphics.beginGradientFill(GradientType.LINEAR, _local_3, _local_4, _local_5, _local_6);
            _local_9.graphics.drawRoundRect(0, 0, btnWidth, btnHeight, _local_7, _local_7);
            _local_9.addChild(createButtonTextField(_arg_1, _arg_2));
            var _local_10:Sprite = new Sprite();
            _local_3 = [brightenColour(_arg_2.colour, -15), brightenColour(_arg_2.colour, 50)];
            _local_10.graphics.lineStyle(1, brightenColour(_arg_2.colour, -50));
            _local_10.graphics.beginGradientFill(GradientType.LINEAR, _local_3, _local_4, _local_5, _local_6);
            _local_10.graphics.drawRoundRect(0, 0, btnWidth, btnHeight, _local_7, _local_7);
            _local_10.addChild(createButtonTextField(_arg_1, _arg_2));
            var _local_11:SimpleButton = new SimpleButton(_local_8, _local_9, _local_10, _local_9);
            _local_11.name = _arg_1;
            return (_local_11);
        }

        private static function getPrompt(_arg_1:AlertOptions):Sprite
        {
            var _local_2:Sprite = createPrompt(_arg_1);
            _local_2.name = "actual_prompt";
            _local_2.addEventListener(MouseEvent.MOUSE_DOWN, doStartDrag);
            _local_2.addEventListener(MouseEvent.MOUSE_UP, doStopDrag);
            if (_arg_1.position)
            {
                _local_2.x = _arg_1.position.x;
                _local_2.y = _arg_1.position.y;
            } else
            {
                _local_2.x = ((stage.stageWidth / 2) - (_local_2.width / 2));
                _local_2.y = ((stage.stageHeight / 2) - (_local_2.height / 2));
            };
            return (_local_2);
        }

        private static function getBlurFilter():BitmapFilter
        {
            var _local_1:Number = 100;
            var _local_2:Number = 100;
            return (new BlurFilter(_local_1, _local_2, BitmapFilterQuality.HIGH));
        }

        private static function getDropShadowFilter(_arg_1:int):DropShadowFilter
        {
            var _local_2:Number = 0;
            var _local_3:Number = 90;
            var _local_4:Number = 0.6;
            var _local_5:Number = 12;
            var _local_6:Number = 4;
            var _local_7:Number = 1;
            var _local_8:Number = 1;
            var _local_9:Boolean;
            var _local_10:Boolean;
            var _local_11:Number = BitmapFilterQuality.LOW;
            return (new DropShadowFilter(_local_7, _local_3, _local_2, _local_4, _local_5, _local_6, _local_8, _local_11, _local_9, _local_10));
        }

        private static function getGlowFilter(_arg_1:int):GlowFilter
        {
            var _local_2:Number = 0xFFFFFF;
            var _local_3:Number = 0.8;
            var _local_4:Number = 15;
            var _local_5:Number = 15;
            var _local_6:Number = 0.7;
            var _local_7:Boolean = true;
            var _local_8:Boolean;
            var _local_9:Number = BitmapFilterQuality.HIGH;
            return (new GlowFilter(_local_2, _local_3, _local_4, _local_5, _local_6, _local_9, _local_7, _local_8));
        }

        private static function createPrompt(_arg_1:AlertOptions):Sprite
        {
            var _local_8:int;
            var _local_10:int;
            var _local_2:Sprite = new Sprite();
            var _local_3:TextField = createTextField(_arg_1);
            var _local_4:int = (_local_3.width + 30);
            var _local_5:int = (_local_3.height + 65);
            if (_local_4 < minimumWidths[(_arg_1.buttons.length - 1)])
            {
                _local_4 = minimumWidths[(_arg_1.buttons.length - 1)];
            };
            if (_local_5 < 100)
            {
                _local_5 = 100;
            };
            if (_local_5 > stage.stageHeight)
            {
                _local_5 = (stage.stageHeight - 20);
                _local_3.autoSize = TextFieldAutoSize.NONE;
                _local_3.height = (stage.stageHeight - 40);
            };
            var _local_6:int = 10;
            _local_2.graphics.lineStyle(1);
            _local_2.graphics.beginFill(_arg_1.colour);
            _local_2.graphics.drawRoundRect(0, 0, _local_4, _local_5, _local_6, _local_6);
            _local_2.graphics.endFill();
            _local_3.x = ((_local_2.width / 2) - (_local_3.width / 2));
            _local_3.y = (((_local_2.height / 2) - (_local_3.height / 2)) - 10);
            var _local_7:Array = new Array();
            while (_local_8 < _arg_1.buttons.length)
            {
                _local_7.push(createButton(_arg_1.buttons[_local_8], _arg_1));
                _local_8++;
            };
            _local_2.filters = [getGlowFilter(_arg_1.colour), getDropShadowFilter(_arg_1.colour)];
            _local_2.alpha = _arg_1.promptAlpha;
            var _local_9:Sprite = new Sprite();
            _local_9.addChild(_local_2);
            switch (_local_7.length)
            {
                case 1:
                    _local_7[0].x = ((_local_9.width / 2) - (btnWidth / 2));
                    break;
                case 2:
                    _local_7[0].x = (((_local_9.width / 2) - btnWidth) - 10);
                    _local_7[1].x = ((_local_7[0].x + btnWidth) + 15);
                    break;
                case 3:
                    _local_7[1].x = ((_local_9.width / 2) - (btnWidth / 2));
                    _local_7[0].x = ((_local_7[1].x - btnWidth) - 15);
                    _local_7[2].x = ((_local_7[1].x + btnWidth) + 15);
                    break;
            };
            _local_9.addChild(_local_3);
            while (_local_10 < _local_7.length)
            {
                _local_7[_local_10].y = (_local_9.height - 35);
                _local_9.addChild(_local_7[_local_10]);
                _local_10++;
            };
            return (_local_9);
        }

        private static function createButtonTextField(_arg_1:String, _arg_2:AlertOptions):TextField
        {
            var _local_3:TextField = new TextField();
            _local_3.textColor = _arg_2.textColour;
            _local_3.selectable = false;
            _local_3.width = btnWidth;
            _local_3.height = btnHeight;
            var _local_4:TextFormat = new TextFormat();
            _local_4.align = TextFormatAlign.CENTER;
            _local_3.defaultTextFormat = _local_4;
            _arg_1 = (("<b>" + _arg_1) + "</b>");
            _local_3.htmlText = (('<font face="Verdana">' + _arg_1) + "</font>");
            _local_3.x = ((btnWidth / 2) - (_local_3.width / 2));
            return (_local_3);
        }

        private static function createTextField(_arg_1:AlertOptions):TextField
        {
            var _local_2:String = _arg_1.text;
            var _local_3:TextField = new TextField();
            _local_3.textColor = _arg_1.textColour;
            _local_3.multiline = true;
            _local_3.selectable = false;
            _local_3.autoSize = TextFieldAutoSize.CENTER;
            _local_3.htmlText = (('<font face="Verdana">' + _local_2) + "</font>");
            _local_3.x = ((btnWidth / 2) - (_local_3.width / 2));
            return (_local_3);
        }

        private static function brightenColour(_arg_1:int, _arg_2:int):int
        {
            var _local_3:Array = hexToRGB(_arg_1);
            var _local_4:int = keepInBounds((_local_3[0] + _arg_2));
            var _local_5:int = keepInBounds((_local_3[1] + _arg_2));
            var _local_6:int = keepInBounds((_local_3[2] + _arg_2));
            return (RGBToHex(_local_4, _local_5, _local_6));
        }

        private static function doStartDrag(_arg_1:MouseEvent):void
        {
            if ((_arg_1.target is Sprite))
            {
                _arg_1.currentTarget.startDrag();
            };
        }

        private static function doStopDrag(_arg_1:MouseEvent):void
        {
            if ((_arg_1.target is Sprite))
            {
                _arg_1.currentTarget.stopDrag();
            };
        }

        private static function hexToRGB(_arg_1:uint):Array
        {
            var _local_2:Array = new Array();
            _local_2.push((_arg_1 >> 16));
            var _local_3:uint = (_arg_1 ^ (_local_2[0] << 16));
            _local_2.push((_local_3 >> 8));
            _local_2.push((_local_3 ^ (_local_2[1] << 8)));
            return (_local_2);
        }

        private static function keepInBounds(_arg_1:int):int
        {
            if (_arg_1 < 0)
            {
                _arg_1 = 0;
            };
            if (_arg_1 > 0xFF)
            {
                _arg_1 = 0xFF;
            };
            return (_arg_1);
        }

        private static function RGBToHex(_arg_1:int, _arg_2:int, _arg_3:int):int
        {
            var _local_4:uint;
            _local_4 = ((_arg_1 & 0xFF) << 16);
            _local_4 = (_local_4 + ((_arg_2 & 0xFF) << 8));
            _local_4 = (_local_4 + (_arg_3 & 0xFF));
            return (_local_4);
        }


    }
}//package 

import flash.geom.Point;

class AlertOptions 
{

    public var background:String;
    public var buttons:Array;
    public var callback:Function;
    public var colour:int;
    public var fadeIn:Boolean;
    public var position:Point;
    public var promptAlpha:Number;
    public var text:String;
    public var textColour:int = 0;

    public function AlertOptions(_arg_1:Object, _arg_2:*):void
    {
        buttons = new Array();
        super();
        if (_arg_1 != null)
        {
            if (_arg_1.background == null)
            {
                background = "simple";
            } else
            {
                background = _arg_1.background;
            };
            if (_arg_1.buttons == null)
            {
                buttons = ["OK"];
            } else
            {
                if (_arg_1.buttons.length > 3)
                {
                    buttons = _arg_1.buttons.slice(0, 2);
                } else
                {
                    buttons = _arg_1.buttons;
                };
            };
            callback = _arg_1.callback;
            if (_arg_1.colour == null)
            {
                colour = 5143985;
            } else
            {
                colour = _arg_1.colour;
            };
            position = _arg_1.position;
            if (_arg_1.promptAlpha == null)
            {
                promptAlpha = 0.9;
            } else
            {
                promptAlpha = _arg_1.promptAlpha;
            };
            if (_arg_1.textColour != null)
            {
                textColour = _arg_1.textColour;
            } else
            {
                textColour = 0;
            };
        } else
        {
            background = "simple";
            buttons = ["OK"];
            colour = 5143985;
            promptAlpha = 0.9;
            textColour = 0;
        };
        text = _arg_2.toString();
    }

}


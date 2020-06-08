package me.zhj.whaledemo;

import android.app.Application;
import android.util.Log;

import com.lody.whale.xposed.XC_MethodHook;
import com.lody.whale.xposed.XposedHelpers;

public class App extends Application {

    public volatile static boolean initedTest = false;

    @Override
    public void onCreate() {
        super.onCreate();

//        XposedHelpers.findAndHookMethod(PendingHookTest.class, "test", new XC_MethodHook() {
//            @Override
//            protected void beforeHookedMethod(MethodHookParam param) throws Throwable {
//                super.beforeHookedMethod(param);
//                Log.d("ZZZ", " call test");
//                param.setResult(null);
//            }
//        });
    }
}

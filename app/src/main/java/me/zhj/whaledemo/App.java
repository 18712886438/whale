package me.zhj.whaledemo;

import android.app.Application;
import android.util.Log;


public class App extends Application {

    public volatile static boolean initedTest = false;

    @Override
    public void onCreate() {
        super.onCreate();
    }
}

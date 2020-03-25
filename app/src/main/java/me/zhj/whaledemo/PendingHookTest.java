package me.zhj.whaledemo;

import android.util.Log;

public class PendingHookTest {

    static {
        if (!App.initedTest) {
            throw new RuntimeException("ZZZ : PendingHookTest.class may can not init this time!");
        }
    }

    private static String ss;
    public PendingHookTest() {
        super();
    }

    public static void test() {
        Log.e("ZZZ", "hook failure!");
    }
}

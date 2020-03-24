package me.zhj.whaledemo;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.Log;

import com.lody.whale.xposed.XC_MethodHook;
import com.lody.whale.xposed.XposedHelpers;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Log.d("ZZZ", "10 + 20  = " + add(10, 20) + " and 10 - 20 = " + sub(10, 20));

        XposedHelpers.findAndHookMethod(MainActivity.class, "add", int.class, int.class, new XC_MethodHook(){
            @Override
            protected void afterHookedMethod(MethodHookParam param) throws Throwable {
                super.afterHookedMethod(param);

                param.setResult(15);
            }
        });

        XposedHelpers.findAndHookMethod(MainActivity.class, "sub", int.class, int.class, new XC_MethodHook(){
            @Override
            protected void afterHookedMethod(MethodHookParam param) throws Throwable {
                super.afterHookedMethod(param);

                param.setResult(15);
            }
        });

        Log.d("ZZZ", "after hook  10 + 20  = " + add(10, 20) + " and 10 - 20 = " + sub(10, 20));
    }

    int add(int a, int b) {

        return a+b;
    }

    int sub(int a, int b) {

        return a-b;
    }
}

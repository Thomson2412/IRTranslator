package com.soloheisbeer.volumechecker

import android.app.Activity
import android.content.Intent
import android.os.Bundle
import android.os.Handler


class MainActivity : Activity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setTheme(R.style.Theme_VolumeChecker)
        setContentView(R.layout.activity_main)

        val intent = Intent(applicationContext, ActionService::class.java)
        intent.setPackage(packageName)
        applicationContext.startForegroundService(intent)

        val handler = Handler();
        handler.postDelayed(Runnable() {
            run() {
                finish()
            }
        }, 2000);
    }
}
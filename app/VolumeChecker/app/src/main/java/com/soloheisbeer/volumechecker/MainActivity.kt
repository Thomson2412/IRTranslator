package com.soloheisbeer.volumechecker

import android.app.Activity
import android.content.Intent
import android.os.Bundle
import android.os.Handler
import android.os.Looper


class MainActivity : Activity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setTheme(R.style.Theme_VolumeChecker)
        setContentView(R.layout.activity_main)

        val startServiceIntent = Intent(applicationContext, ActionService::class.java)
        applicationContext.startForegroundService(startServiceIntent)

        val handler = Handler(Looper.getMainLooper());
        handler.postDelayed({
            finish()
        }, 2000);
    }
}
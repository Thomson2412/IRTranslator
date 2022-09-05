package com.soloheisbeer.volumechecker

import android.util.Log
import java.net.HttpURLConnection
import java.net.URL
import java.util.concurrent.ExecutorService
import java.util.concurrent.Executors


class NetworkManager() {
    private var espBaseUrl = "http://192.168.178.87/"
    private val executor: ExecutorService = Executors.newSingleThreadExecutor()

    fun powerOn() {
        sendPostRequest("poweron")
    }
    fun powerOff() {
        sendPostRequest("poweroff")

    }
    fun volumeUp(amount: Int) {
        sendPostRequest("volumeup?amount=$amount")

    }
    fun volumeDown(amount: Int) {
        sendPostRequest("volumedown?amount=$amount")
    }

    fun mute() {
        sendPostRequest("mute")
    }

    private fun sendPostRequest(urlPart: String){
        executor.execute {
            try {
                val postUrl = URL(espBaseUrl + urlPart)
                val connection = postUrl.openConnection() as HttpURLConnection
                connection.requestMethod = "POST"
                connection.connect()
                val responseCode = connection.responseCode
                Log.d("NetworkManger", "$urlPart: $responseCode")
            } catch (e: Exception) {
                e.printStackTrace()
            }
        }
    }
}
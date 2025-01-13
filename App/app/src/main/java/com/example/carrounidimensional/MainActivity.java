package com.example.carrounidimensional;

import android.annotation.SuppressLint;
import android.bluetooth.*;
import android.content.Context;
import android.os.Bundle;
import android.util.Log;
import android.widget.ImageButton;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import java.util.UUID;

public class MainActivity extends AppCompatActivity {

    private static final String TAG = "MainActivity";

    // UUIDs do serviço e característica
    private static final UUID SERVICE_UUID = UUID.fromString("014dd1c0-9dde-4907-8c31-e7d6b7a77ddb");
    private static final UUID CHARACTERISTIC_CON_UUID = UUID.fromString("c7be25a0-d82d-44e0-8155-3cbac410e2ed");
    private static final UUID BUZZ_UUID = UUID.fromString("1e29d664-837a-42cd-8051-451e8985c08b");

    private ImageButton button, buttonesquerda, buttondireita, buttonbaixo;
    private ImageButton north_west, north_east, south_weast, south_east, stop;
    private ImageButton rotacionar_direita, rotacionar_esquerda, buzina;
    private BluetoothAdapter mBluetoothAdapter;
    private BluetoothGatt mBluetoothGatt;

    // Dados para envio
    private final byte[] data_to_write_cima = "x1y0r0".getBytes();
    private final byte[] data_to_write_esquerda = "x0y1r0".getBytes();
    private final byte[] data_to_write_baixo = "x-1y0r0".getBytes();
    private final byte[] data_to_write_direita = "x0y-1r0".getBytes();
    private final byte[] data_to_write_diagonal_north_weast = "x1y1r0".getBytes();
    private final byte[] data_to_write_diagonal_north_east = "x1y-1r0".getBytes();
    private final byte[] data_to_write_diagonal_south_east = "x-1y-1r0".getBytes();
    private final byte[] data_to_write_diagonal_south_weast = "x-1y1r0".getBytes();
    private final byte[] data_to_write_diagonal_stop = "x0y0r0".getBytes();
    private final byte[] data_to_write_diagonal_rotacionar_esquerda = "x1y1r1".getBytes();
    private final byte[] data_to_write_diagonal_rotacionar_direita = "x1y-1r1".getBytes();

    @SuppressLint("MissingPermission")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Inicializa os botões
        button = findViewById(R.id.cima);
        buttonesquerda = findViewById(R.id.esquerda);
        buttondireita = findViewById(R.id.direita);
        buttonbaixo = findViewById(R.id.baixo);
        north_east = findViewById(R.id.diagonal_north_east);
        north_west = findViewById(R.id.diagonal_north_weast);
        south_east = findViewById(R.id.diagonal_south_east);
        south_weast = findViewById(R.id.diagonal_south_weast);
        stop = findViewById(R.id.stop);
        rotacionar_direita = findViewById(R.id.rotacionar_direita);
        rotacionar_esquerda = findViewById(R.id.rotacionar_esquerda);
        buzina = findViewById(R.id.buzina);

        // Configuração do Bluetooth
        BluetoothManager mBluetoothManager = (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);
        if (mBluetoothManager == null) {
            Toast.makeText(this, "Erro ao inicializar o Bluetooth Manager.", Toast.LENGTH_SHORT).show();
            return;
        }

        mBluetoothAdapter = mBluetoothManager.getAdapter();
        if (mBluetoothAdapter == null || !mBluetoothAdapter.isEnabled()) {
            Toast.makeText(this, "Bluetooth desativado. Ative o Bluetooth para continuar.", Toast.LENGTH_SHORT).show();
            return;
        }

        // Conexão com o dispositivo
        String deviceAddress = "48:31:B7:04:9A:36"; // Substitua pelo endereço correto
        BluetoothDevice mDevice = mBluetoothAdapter.getRemoteDevice(deviceAddress);
        if (mDevice == null) {
            Toast.makeText(this, "Dispositivo Bluetooth não encontrado.", Toast.LENGTH_SHORT).show();
            return;
        }

        mBluetoothGatt = mDevice.connectGatt(this, false, gattCallback);

        // Configurações dos botões
        configureButton(button, data_to_write_cima);
        configureButton(buttonesquerda, data_to_write_esquerda);
        configureButton(buttondireita, data_to_write_direita);
        configureButton(buttonbaixo, data_to_write_baixo);
        configureButton(north_west, data_to_write_diagonal_north_weast);
        configureButton(north_east, data_to_write_diagonal_north_east);
        configureButton(south_weast, data_to_write_diagonal_south_weast);
        configureButton(south_east, data_to_write_diagonal_south_east);
        configureButton(stop, data_to_write_diagonal_stop);
        configureButton(rotacionar_direita, data_to_write_diagonal_rotacionar_direita);
        configureButton(rotacionar_esquerda, data_to_write_diagonal_rotacionar_esquerda);

        buzina.setOnClickListener(view -> {
            if (mBluetoothGatt != null) {
                BluetoothGattService mService = mBluetoothGatt.getService(SERVICE_UUID);
                if (mService != null) {
                    BluetoothGattCharacteristic mCharacteristic = mService.getCharacteristic(BUZZ_UUID);
                    if (mCharacteristic != null) {
                        mCharacteristic.setValue("G4".getBytes());
                        mBluetoothGatt.writeCharacteristic(mCharacteristic);
                    }
                }
            }
        });
    }

    private void configureButton(ImageButton button, byte[] data) {
        button.setOnClickListener(view -> {
            if (mBluetoothGatt != null) {
                BluetoothGattService mService = mBluetoothGatt.getService(SERVICE_UUID);
                if (mService != null) {
                    BluetoothGattCharacteristic mCharacteristic = mService.getCharacteristic(CHARACTERISTIC_CON_UUID);
                    if (mCharacteristic != null) {
                        mCharacteristic.setValue(data);
                        mBluetoothGatt.writeCharacteristic(mCharacteristic);
                    }
                }
            }
        });
    }

    private final BluetoothGattCallback gattCallback = new BluetoothGattCallback() {
        @Override
        public void onConnectionStateChange(BluetoothGatt gatt, int status, int newState) {
            super.onConnectionStateChange(gatt, status, newState);
            if (newState == BluetoothProfile.STATE_CONNECTED) {
                Log.i(TAG, "Conectado ao dispositivo GATT.");
                gatt.discoverServices();
            } else if (newState == BluetoothProfile.STATE_DISCONNECTED) {
                Log.i(TAG, "Desconectado do dispositivo GATT.");
                mBluetoothGatt = null;
            }
        }

        @Override
        public void onServicesDiscovered(BluetoothGatt gatt, int status) {
            if (status == BluetoothGatt.GATT_SUCCESS) {
                Log.i(TAG, "Serviços descobertos com sucesso.");
            } else {
                Log.w(TAG, "Falha ao descobrir os serviços: " + status);
            }
        }
    };

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (mBluetoothGatt != null) {
            mBluetoothGatt.close();
            mBluetoothGatt = null;
        }
    }
}

package com.example.jnidemo;

import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.widget.Button;   // AJOUT : Import indispensable pour le bouton
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    // Déclarations des méthodes natives
    public native String helloFromJNI();
    public native int factorial(int n);
    public native String reverseString(String s);
    public native int sumArray(int[] values);
    public native float[] multiplyMatrices(float[] a, float[] b, int size);
    public native boolean isSafeString(String input);
    public native int fastAdd(int a, int b); // Enregistrée via RegisterNatives (Extension D)

    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Liaison des vues
        TextView tvHello = findViewById(R.id.tvHello);
        TextView tvFact = findViewById(R.id.tvFact);
        TextView tvReverse = findViewById(R.id.tvReverse);
        TextView tvArray = findViewById(R.id.tvArray);

        // Liaison des vues des extensions
        TextView tvSafe = findViewById(R.id.tvSafeString);
        TextView tvFast = findViewById(R.id.tvFastAdd);
        TextView tvBench = findViewById(R.id.tvBenchmark);
        Button btnBench = findViewById(R.id.btnRunBenchmark);

        // 1. Hello World
        tvHello.setText(helloFromJNI());

        // 2. Factoriel
        int fact10 = factorial(10);
        tvFact.setText(fact10 >= 0 ? "Factoriel de 10 = " + fact10 : "Erreur factoriel : " + fact10);

        // 3. Reverse String
        tvReverse.setText("Texte inversé : " + reverseString("JNI is powerful!"));

        // 4. Somme Tableau
        int[] numbers = {10, 20, 30, 40, 50};
        tvArray.setText("Somme du tableau = " + sumArray(numbers));

        // 5. Test Sécurité (Extension B)
        boolean safe = isSafeString("Hello; DROP TABLE");
        tvSafe.setText("Sécurité : " + (safe ? "Sûre" : "Danger détecté !"));
        Log.i("JNI_DEMO", "La chaîne est-elle sûre ? " + safe);

        // 6. Test RegisterNatives (Extension D)
        int resultAdd = fastAdd(5, 10);
        tvFast.setText("Addition Dynamique (5+10) = " + resultAdd);
        Log.i("JNI_DEMO", "Résultat fastAdd (dynamique) : " + resultAdd);

        // 7. Benchmark via Bouton (Extension C)
        btnBench.setOnClickListener(v -> {
            tvBench.setText("Calcul en cours...");
            runBenchmark(tvBench);
        });
    }

    // Méthode de benchmark mise à jour pour afficher le résultat sur l'écran
    public void runBenchmark(TextView display) {
        int size = 64;
        float[] m1 = new float[size * size];
        float[] m2 = new float[size * size];

        for(int i = 0; i < m1.length; i++) m1[i] = (float) i;

        long start = System.nanoTime();
        multiplyMatrices(m1, m2, size); // Extension A
        long end = System.nanoTime();

        long duration = end - start;
        display.setText("Benchmark : " + duration + " ns");
        Log.i("JNI_DEMO", "Temps Native Matrix Mult: " + duration + " ns");
    }
}
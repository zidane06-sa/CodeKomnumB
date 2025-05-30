#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Struktur untuk menyimpan parameter model
typedef struct {
    double r;        // Growth rate (laju pertumbuhan intrinsik)
    double K;        // Carrying capacity (kapasitas lingkungan)
    double P0;       // Populasi awal
    double t_max;    // Waktu simulasi maksimum
    double dt;       // Step size
} PopulationParams;

// Fungsi diferensial untuk Logistic Growth Model
// dP/dt = r * P * (1 - P/K)
double logistic_growth(double t, double P, PopulationParams *params) {
    return params->r * P * (1.0 - P / params->K);
}

// Implementasi Runge-Kutta Order 4 (RK4)
double runge_kutta_4(double t, double P, double dt, PopulationParams *params) {
    double k1, k2, k3, k4;
    
    k1 = dt * logistic_growth(t, P, params);
    k2 = dt * logistic_growth(t + dt/2.0, P + k1/2.0, params);
    k3 = dt * logistic_growth(t + dt/2.0, P + k2/2.0, params);
    k4 = dt * logistic_growth(t + dt, P + k3, params);
    
    return P + (k1 + 2*k2 + 2*k3 + k4) / 6.0;
}

// Fungsi untuk menjalankan simulasi
void run_simulation(PopulationParams *params) {
    double t = 0.0;
    double P = params->P0;
    int steps = (int)(params->t_max / params->dt);
    
    printf("\n=== SIMULASI PERTUMBUHAN POPULASI ===\n");
    printf("Model: Logistic Growth (dP/dt = r*P*(1-P/K))\n");
    printf("Parameters:\n");
    printf("- Growth rate (r): %.4f\n", params->r);
    printf("- Carrying capacity (K): %.0f\n", params->K);
    printf("- Initial population (P0): %.0f\n", params->P0);
    printf("- Time step (dt): %.4f\n", params->dt);
    printf("\n");
    
    printf("%-10s %-15s %-15s %-15s\n", "Time", "Population", "Growth Rate", "% of K");
    printf("%-10s %-15s %-15s %-15s\n", "----", "----------", "-----------", "------");
    
    // Simpan data untuk output file
    FILE *output_file = fopen("population_data.csv", "w");
    if (output_file != NULL) {
        fprintf(output_file, "Time,Population,GrowthRate,PercentageOfK\n");
    }
    
    for (int i = 0; i <= steps; i++) {
        double growth_rate = logistic_growth(t, P, params);
        double percentage_K = (P / params->K) * 100.0;
        
        // Print setiap 10 step untuk menghindari output terlalu panjang
        if (i % 10 == 0) {
            printf("%-10.2f %-15.2f %-15.4f %-15.2f%%\n", 
                   t, P, growth_rate, percentage_K);
        }
        
        // Simpan ke file
        if (output_file != NULL) {
            fprintf(output_file, "%.4f,%.4f,%.6f,%.4f\n", 
                    t, P, growth_rate, percentage_K);
        }
        
        // Update menggunakan RK4
        P = runge_kutta_4(t, P, params->dt, params);
        t += params->dt;
        
        // Cek jika populasi sudah mencapai carrying capacity
        if (P >= params->K * 0.999) {
            printf("\n>>> Populasi mencapai 99.9%% dari carrying capacity pada t = %.2f\n", t);
            break;
        }
    }
    
    if (output_file != NULL) {
        fclose(output_file);
        printf("\nData simulasi disimpan dalam file: population_data.csv\n");
    }
}

// Fungsi untuk analisis stabilitas
void analyze_model(PopulationParams *params) {
    printf("\n=== ANALISIS MODEL ===\n");
    
    // Titik kesetimbangan
    printf("Titik kesetimbangan:\n");
    printf("- P = 0 (populasi punah)\n");
    printf("- P = K = %.0f (carrying capacity)\n", params->K);
    
    // Waktu untuk mencapai 50% carrying capacity
    double t_half = log(params->K / params->P0 - 1.0) / params->r;
    if (t_half > 0) {
        printf("\nWaktu untuk mencapai 50%% carrying capacity: %.2f unit waktu\n", t_half);
    }
    
    // Maximum growth rate
    double P_max_growth = params->K / 2.0;
    double max_growth_rate = params->r * P_max_growth * (1.0 - P_max_growth / params->K);
    printf("Maximum growth rate: %.4f pada P = %.0f\n", max_growth_rate, P_max_growth);
}

// Fungsi untuk input parameter dari user
void input_parameters(PopulationParams *params) {
    printf("=== INPUT PARAMETER MODEL ===\n");
    
    printf("Masukkan growth rate (r) [contoh: 0.1]: ");
    scanf("%lf", &params->r);
    
    printf("Masukkan carrying capacity (K) [contoh: 1000]: ");
    scanf("%lf", &params->K);
    
    printf("Masukkan populasi awal (P0) [contoh: 50]: ");
    scanf("%lf", &params->P0);
    
    printf("Masukkan waktu simulasi maksimum [contoh: 50]: ");
    scanf("%lf", &params->t_max);
    
    printf("Masukkan step size (dt) [contoh: 0.1]: ");
    scanf("%lf", &params->dt);
    
    // Validasi input
    if (params->r <= 0 || params->K <= 0 || params->P0 <= 0 || 
        params->t_max <= 0 || params->dt <= 0) {
        printf("Error: Semua parameter harus bernilai positif!\n");
        exit(1);
    }
    
    if (params->P0 >= params->K) {
        printf("Warning: Populasi awal >= Carrying capacity!\n");
    }
}

// Fungsi untuk menampilkan contoh skenario
void show_example_scenarios() {
    printf("\n=== CONTOH SKENARIO ===\n");
    printf("1. Pertumbuhan Bakteri:\n");
    printf("   r = 0.5, K = 1000, P0 = 10\n");
    printf("2. Pertumbuhan Populasi Kota:\n");
    printf("   r = 0.03, K = 100000, P0 = 5000\n");
    printf("3. Pertumbuhan Ikan di Kolam:\n");
    printf("   r = 0.2, K = 500, P0 = 20\n");
}

int main() {
    PopulationParams params;
    int choice;
    
    printf("==============================================\n");
    printf("    SIMULASI PERTUMBUHAN POPULASI\n");
    printf("      Logistic Growth Model\n");
    printf("==============================================\n");
    
    printf("\nPilih mode:\n");
    printf("1. Input parameter manual\n");
    printf("2. Gunakan contoh skenario\n");
    printf("3. Lihat contoh skenario\n");
    printf("Pilihan: ");
    scanf("%d", &choice);
    
    switch(choice) {
        case 1:
            input_parameters(&params);
            break;
            
        case 2:
            printf("\nPilih skenario:\n");
            printf("1. Pertumbuhan Bakteri (r=0.5, K=1000, P0=10)\n");
            printf("2. Pertumbuhan Kota (r=0.03, K=100000, P0=5000)\n");
            printf("3. Pertumbuhan Ikan (r=0.2, K=500, P0=20)\n");
            printf("Pilihan: ");
            int scenario;
            scanf("%d", &scenario);
            
            params.t_max = 50.0;
            params.dt = 0.1;
            
            switch(scenario) {
                case 1:
                    params.r = 0.5; params.K = 1000; params.P0 = 10;
                    break;
                case 2:
                    params.r = 0.03; params.K = 100000; params.P0 = 5000;
                    params.t_max = 200.0;
                    break;
                case 3:
                    params.r = 0.2; params.K = 500; params.P0 = 20;
                    break;
                default:
                    printf("Pilihan tidak valid!\n");
                    return 1;
            }
            break;
            
        case 3:
            show_example_scenarios();
            return 0;
            
        default:
            printf("Pilihan tidak valid!\n");
            return 1;
    }
    
    // Jalankan simulasi
    analyze_model(&params);
    run_simulation(&params);
    
    printf("\n=== INTERPRETASI HASIL ===\n");
    printf("- Kurva S-shaped menunjukkan pertumbuhan logistik\n");
    printf("- Pertumbuhan cepat di awal, melambat mendekati K\n");
    printf("- Carrying capacity adalah batas maksimum populasi\n");
    printf("- Growth rate maksimum terjadi pada P = K/2\n");
    
    return 0;
}
// Test Harness
public class T {

    "" => string context;

    fun static void println(string s) {
        chout <= s <= IO.nl();
    }
    
    fun static void err(string s) {
        cherr <= s <= IO.nl();
    }

    fun static void assert(int bool) {
        if (!bool) err("Assertion failed");
    }

    fun static void assert(int bool, string s) {
        if (!bool) err("Assertion failed: " + s);
    }

    // causes segfault. chuck bug
    // fun static int strEquals(string a, string b) {
    //     // chuck can't compare a null string to a non-null string 
    //     if (a == null && b == null) {
    //         return 1;
    //     }


    //     false => int equals;
    //     if (a == null || b == null) {
    //         false => equals;
    //     } else {
    //         (a == b) => equals;
    //     }

    //     if (!equals) {
    //         // because chuck can't concatenate null strings
    //         err("Strings not equal:");
    //         <<< a >>>;
    //         err("!=");
    //         <<< b >>>;
    //     }

    //     return equals;
    // }


    fun static int feq(float a, float b) {
        return Math.fabs(a - b) < 0.0001;
    }

    fun static int veq(vec2 a, vec2 b) {
        return T.feq(a.x, b.x) && T.feq(a.y, b.y);
    }

    fun static int veq(vec3 a, vec3 b) {
        return T.feq(a.x, b.x) && T.feq(a.y, b.y) && T.feq(a.z, b.z);
    }

    fun static int veq(vec4 a, vec4 b) {
        return T.feq(a.x, b.x) && T.feq(a.y, b.y) && T.feq(a.z, b.z) && T.feq(a.w, b.w);
    }

    fun void assert(string code) {
        Machine.eval(
            this.context + 
            " T.assert(" + code + ", \"" + code + "\"); "
        );
    }

    fun static void printArray(float arr[], int num_components) {
        chout <= "[ ";
        for (0 => int i; i < arr.size(); num_components +=> i) {
            if (num_components > 1) {
                chout <= "( ";
            }
            for (i => int j; j < i + num_components; ++j) {
                chout <= arr[j] <= ", ";
            }
            if (num_components > 1) {
                chout <= ") ";
            }
        }
        chout <= " ]" <= IO.nl();
    }

    fun static void printArray(int arr[], int num_components) {
        chout <= "[ ";
        for (0 => int i; i < arr.size(); num_components +=> i) {
            if (num_components > 1) {
                chout <= "( ";
            }
            for (i => int j; j < i + num_components; ++j) {
                chout <= arr[j] <= ", ";
            }
            if (num_components > 1) {
                chout <= ") ";
            }
        }
        chout <= "]" <= IO.nl();
    }

    fun static void printArray(vec2 arr[]) {
        chout <= "[ ";
        for (0 => int i; i < arr.size(); ++i) {
            chout <= "( " <= arr[i].x <= ", " <= arr[i].y <= " ), ";
        }
        chout <= "]" <= IO.nl();
    }
    fun static void print(vec2 arr[]) { printArray(arr); }

    fun static void printArray(vec3 arr[]) {
        chout <= "[ ";
        for (0 => int i; i < arr.size(); ++i) {
            chout <= "( " <= arr[i].x <= ", " <= arr[i].y <= ", " <= arr[i].z <= " ), ";
        }
        chout <= "]" <= IO.nl();
    }

    fun static void printArray(vec4 arr[]) {
        chout <= "[ ";
        for (0 => int i; i < arr.size(); ++i) {
            chout <= "( " <= arr[i].x <= ", " <= arr[i].y <= ", " <= arr[i].z <= ", " <= arr[i].w <= " ), ";
        }
        chout <= "]" <= IO.nl();
    }

    fun static void printArray(int arr[]) {
        T.printArray(arr, 1);
    }

    fun static void print(int arr[]) {
        T.printArray(arr, 1);
    }

    fun static void printArray(float arr[]) {
        T.printArray(arr, 1);
    }

    fun static void print(string arr[]) {
        chout <= "[ ";
        for (0 => int i; i < arr.size(); ++i) {
            chout <= arr[i] <= ", ";
        }
        chout <= "]" <= IO.nl();
    }

    fun static int arrayHas(Object arr[], Object e) {
        for (auto a : arr) {
            if (a == e) return true;
        }
        return false;
    }

    fun static int arrayHas(int arr[], int e) {
        for (auto a : arr) {
            if (a == e) return true;
        }
        return false;
    }

    fun static int arrayEquals(int a[], int b[]) {
        if (a == null && b == null) {
            return 1;
        }
        if (a == null || b == null) {
            return 0;
        }
        if (a.size() != b.size()) {
            return 0;
        }
        for (0 => int i; i < a.size(); ++i) {
            if (a[i] != b[i]) {
                return 0;
            }
        }
        return 1;
    }

    fun static int arrayEquals(float a[], float b[]) {
        if (a == null && b == null) {
            return 1;
        }
        if (a == null || b == null) {
            return 0;
        }
        if (a.size() != b.size()) {
            return 0;
        }
        for (0 => int i; i < a.size(); ++i) {
            if (!T.feq(a[i], b[i])) {
                <<< a[i], " != ", b[i], " at index ", i >>>;
                T.println("Arrays not equal:");
                T.printArray(a);
                T.printArray(b);
                return 0;
            }
        }
        return 1;
    }

    fun static int arrayEquals(vec2 a[], vec2 b[]) {
        if (a == null && b == null) {
            return 1;
        }
        if (a == null || b == null) {
            return 0;
        }
        if (a.size() != b.size()) {
            return 0;
        }
        for (0 => int i; i < a.size(); ++i) {
            if (!T.veq(a[i], b[i])) {
                <<< a[i], " != ", b[i], " at index ", i >>>;
                T.println("Arrays not equal:");
                T.printArray(a);
                T.printArray(b);
                return 0;
            }
        }
        return 1;
    }

    fun static int arrayEquals(vec3 a[], vec3 b[]) {
        if (a == null && b == null) {
            return 1;
        }
        if (a == null || b == null) {
            return 0;
        }
        if (a.size() != b.size()) {
            return 0;
        }
        for (0 => int i; i < a.size(); ++i) {
            if (!T.veq(a[i], b[i])) {
                <<< a[i], " != ", b[i], " at index ", i >>>;
                T.println("Arrays not equal:");
                T.printArray(a);
                T.printArray(b);
                return 0;
            }
        }
        return 1;
    }

    fun static int arrayEquals(vec4 a[], vec4 b[]) {
        if (a == null && b == null) {
            return 1;
        }
        if (a == null || b == null) {
            return 0;
        }
        if (a.size() != b.size()) {
            return 0;
        }
        for (0 => int i; i < a.size(); ++i) {
            if (!T.veq(a[i], b[i])) {
                <<< a[i], " != ", b[i], " at index ", i >>>;
                T.println("Arrays not equal:");
                T.printArray(a);
                T.printArray(b);
                return 0;
            }
        }
        return 1;
    }
}

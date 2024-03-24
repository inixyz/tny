# principles
- static scoping
- std::vector of Data

- program_run iterates through the vecotor for data from parse and runs eval on each item
    returns what the last item evaluates to


headear only with define implementation

# C
printf("Hello, World!\n");

# tny.lsp
(print "Hello, World!\n")


# C
int x = 5;
char* y = "Salut";

# tny.lsp
(let x 5)
(let y "Salut")

# C
if (x > 5) x += 10;

# tny.lsp
(if (> x 5) (= x (+ x 10)))


# C
double sum(double a, double b) {
    return a + b;
}

# tny.lsp
(fn sum '(a b) (+ a b))


# C
double factorial(double n) {
    if (n > 1) return n * factorial(n - 1);
    return 1;
}

# tny.lsp
(fn factorial '(n)
    (if (> n 1)
        (factorial (- n 1))
        1
    )
)

(= sum (fn (a b) (+ a b)))
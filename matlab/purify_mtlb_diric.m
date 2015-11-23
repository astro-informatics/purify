function diric = purify_mtlb_diric(k, N)
    diric = 0 * k;
	f = sin(k);
	i = abs(f) > 1e-12; % nonzero denominator
	diric(i) = sin(N*k(i)) ./ (N * f(i));
	diric(~i) = sign(cos(k(~i)*(N-1)));
end
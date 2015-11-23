function interpolation_matrix = purify_mtlb_init_interpolation_matrix(UV, kernelu, kernelv, FTsize, Ju, Jv, parallel_type)
%Function to generate interpolation matrix
%UV are the coordinates of the visibilities
%kernel is the function for the interpolation kernel, should be a function
%of u and j only.
%FTsize is the size of the fft grid
%Ju number of nearest neighbours in axis 1
%Jv number of nearest neighbours in axis 2
%parallel_type can take values 1 or 2. Choosing 1 means values of j are
%calculated in parallel for all values of m. Choosing 2 means values of m
%are calculated in parallel for a given value of m.

FTu = FTsize(1);
FTv = FTsize(2);
% visibility coordinates
U = UV(:, 1);
V = UV(:, 2);
% visibility coordinates mapped to integers
um = purify_mtlb_omega_to_k(FTu, U, Ju);
vm = purify_mtlb_omega_to_k(FTv, V, Jv);
% Number of visibilities
M = length(um);


% Parallel method 1, where Ju and Jv are done in parallel, and M is
% vectorized.
if parallel_type == 1
    %Temp variables for parallel loop.
    temp_interp = cell(Ju, Jv);
    temp_index = cell(Ju, Jv);
    % Using a parrallel loop to quickly calculating gridding matrix
    % interpolation kernel.
    parfor ju = 1:Ju
        i = mod(um+ju,FTu)+1;
        for jv = 1:Jv
            fprintf('Generating interpolation matrix entry (%d %d)\n', ju, jv)
            j = mod(vm+jv,FTv)+1;
            cols = sub2ind([FTu, FTv], i, j); % Columns for each value of (u+ju, v+jv) at each visibility
            index = sub2ind([M,FTu*FTv], 1:M, cols'); % Convert the columns into an index for sparse matrix
            temp_index{ju, jv} = index;
            temp_interp{ju, jv} =+ kernelu(U-um,ju).*kernelv(V-vm,jv);
        end
    end
    %Storing temp variables into sparse matrix structure.
    
    for ju = 1:Ju
        for jv = 1:Jv
            interpolation_matrix(temp_index{ju, jv}) = temp_interp{ju, jv};
        end
    end
end

if parallel_type == 2
    %Temp variables for parallel loop.
    temp_interp = zeros(Ju, Jv, M);
    temp_index = zeros(Ju, Jv, M);
    % Using a parrallel loop to quickly calculating gridding matrix
    % interpolation kernel.
    fprintf('Using a support of (%d %d)\n', Ju, Jv)
    for ju = 1:Ju
        for jv = 1:Jv
            fprintf('Generating interpolation matrix entry (%d %d)\n', ju, jv)
            parfor m = 1:M
                i = mod(um(m)+ju,FTu)+1;
                j = mod(vm(m)+jv,FTv)+1;
                cols = sub2ind([FTu, FTv], i, j); % Columns for each value of (u+ju, v+jv) at each visibility
                index = sub2ind([M,FTu*FTv], m, cols'); % Convert the columns into an index for sparse matrix
                temp_index(ju, jv, m) = index;
                temp_interp(ju, jv, m) = kernelu(U(m)-um(m),ju).*kernelv(V(m)-vm(m),jv);
            end
        end
    end
    %Storing temp variables into sparse matrix structure.
    fprintf('Finished generating interpolation matrix\n')
%     parfor ju = 1:Ju
%         for jv = 1:Jv
%             for m = 1:M
%                 interpolation_matrix(temp_index{ju, jv, m}) = temp_interp{ju, jv, m};
%             end
%         end
%     end
    [I, J] = ind2sub([M,FTu*FTv], reshape(temp_index,Ju*Jv*M,1));
    clear temp_index;
    fprintf('Storing interpolation matrix\n')
    interpolation_matrix = sparse(I, J, reshape(temp_interp,Ju*Jv*M,1), M, FTu*FTv);
    
end

end
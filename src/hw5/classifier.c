#include <math.h>
#include <stdlib.h>
#include "image.h"
#include "matrix.h"

// Run an activation function on each element in a matrix,
// modifies the matrix in place
// matrix m: Input to activation function
// ACTIVATION a: function to run
void activate_matrix(matrix m, ACTIVATION a)
{
    int i, j;
    for(i = 0; i < m.rows; ++i){
        double sum = 0;
        for(j = 0; j < m.cols; ++j){
            float x = m.data[i][j];
            if(a == LOGISTIC){
                x = 1.0f + expf(-x);
                x = 1.0f / x;
                // TODONE
                m.data[i][j] = x;
            } else if (a == RELU){
                // TODONE
                if (x <= 0) {
                    x = 0;
                }
                m.data[i][j] = x;
            } else if (a == LRELU){
                if (x < 0) { 
                    x *= 0.1f;
                }
                m.data[i][j] = x;
                // TODONE
            } else if (a == SOFTMAX){
                // TODONE
                m.data[i][j] = expf(x);
            }
            sum += m.data[i][j];
        }
        if (a == SOFTMAX) {
            // TODONE: have to normalize by sum if we are using SOFTMAX
            for(j = 0; j < m.cols; ++j){
                double x = m.data[i][j];
                m.data[i][j] = x / sum;
            }
        }
    }
}

// Calculates the gradient of an activation function and multiplies it into
// the delta for a layer
// matrix m: an activated layer output
// ACTIVATION a: activation function for a layer
// matrix d: delta before activation gradient
void gradient_matrix(matrix m, ACTIVATION a, matrix d)
{
    int i, j;
    for(i = 0; i < m.rows; ++i){
        for(j = 0; j < m.cols; ++j){
            double x = m.data[i][j];
            double res = 0.0;
            if (a == LOGISTIC) {
               res = x * (1-x); 
            } else if (a == RELU) {
                // TODONE
                res = x > 0;
            } else if (a == LRELU) {
                // TODONE
                if (x > 0) {
                  res = 1;
                } else {
                  res = 0.1;
                }
            } else if (a == SOFTMAX) {
              res = 1.0;
            }
            // double dx = d.data[i][j];
            d.data[i][j] *= res;
            // TODONE: multiply the correct element of d by the gradient
        }
    }
}

// Forward propagate information through a layer
// layer *l: pointer to the layer
// matrix in: input to layer
// returns: matrix that is output of the layer
matrix forward_layer(layer *l, matrix in)
{

    l->in = in;  // Save the input for backpropagation
    

    // TODONE: fix this! multiply input by weights and apply activation function.
    //matrix out = make_matrix(in.rows, l->w.cols);
    ACTIVATION a = l->activation;
    matrix w = l->w;
    matrix out = matrix_mult_matrix(in, w);
    activate_matrix(out, a);



    free_matrix(l->out);// free the old output
    l->out = out;       // Save the current output for gradient calculation
    return out;
}

// Backward propagate derivatives through a layer
// layer *l: pointer to the layer
// matrix delta: partial derivative of loss w.r.t. output of layer
// returns: matrix, partial derivative of loss w.r.t. input to layer
matrix backward_layer(layer *l, matrix delta)
{
    // 1.4.1
    // delta is dL/dy
    // TODONE: modify it in place to be dL/d(xw)
    gradient_matrix(l->out, l->activation, delta);


    // 1.4.2
    // TODONE: then calculate dL/dw and save it in l->dw
    free_matrix(l->dw);
    matrix xt = transpose_matrix(l->in);
    matrix dw = matrix_mult_matrix(xt, delta);
    free_matrix(xt);

    l->dw = dw;

    
    // 1.4.3
    // TODONE: finally, calculate dL/dx and return it.
    matrix wt = transpose_matrix(l->w);
    matrix dx = matrix_mult_matrix(delta, wt);
    free_matrix(wt);

    return dx;
}

static matrix matrix_mult_scalar(matrix in, double scalar) {
    matrix res = copy_matrix(in);
    for (int i = 0; i < in.rows; i++) {
      for (int j = 0; j < in.cols; j++) {
        res.data[i][j] *= scalar;
      }
    }
    return res;
}

static matrix matrix_add_matrix(matrix a, matrix b) {
    matrix res = copy_matrix(a);
    for (int i = 0; i < a.rows; i++) {
      for (int j = 0; j < a.cols; j++) {
        res.data[i][j] += b.data[i][j];
      }
    }
    return res;
}

// Update the weights at layer l
// layer *l: pointer to the layer
// double rate: learning rate
// double momentum: amount of momentum to use
// double decay: value for weight decay
void update_layer(layer *l, double rate, double momentum, double decay)
{
    // TODONE:
    // Calculate Δw_t = dL/dw_t - λw_t + mΔw_{t-1}
    //                  l->dw - decay*l->w   + momentum * l->v
    matrix decmat = matrix_mult_scalar(l->w, -1.0 * decay);
    matrix moment = matrix_mult_scalar(l->v, momentum);
    matrix temp = matrix_add_matrix(l->dw, decmat);
    matrix delta_w = matrix_add_matrix(temp, moment);
    free_matrix(temp);
    free_matrix(moment);
    free_matrix(decmat);

    // save it to l->v
    free_matrix(l->v);
    l->v = delta_w;


    // Update l->w
    // l->w = l->w + rate * l->v
    temp = matrix_mult_scalar(l->v, rate);
    matrix temp2 = matrix_add_matrix(l->w, temp);
    free_matrix(l->w);
    free_matrix(temp);
    l->w = temp2;


    // Remember to free any intermediate results to avoid memory leaks

}

// Make a new layer for our model
// int input: number of inputs to the layer
// int output: number of outputs from the layer
// ACTIVATION activation: the activation function to use
layer make_layer(int input, int output, ACTIVATION activation)
{
    layer l;
    l.in  = make_matrix(1,1);
    l.out = make_matrix(1,1);
    l.w   = random_matrix(input, output, sqrt(2./input));
    l.v   = make_matrix(input, output);
    l.dw  = make_matrix(input, output);
    l.activation = activation;
    return l;
}

// Run a model on input X
// model m: model to run
// matrix X: input to model
// returns: result matrix
matrix forward_model(model m, matrix X)
{
    int i;
    for(i = 0; i < m.n; ++i){
        X = forward_layer(m.layers + i, X);
    }
    return X;
}

// Run a model backward given gradient dL
// model m: model to run
// matrix dL: partial derivative of loss w.r.t. model output dL/dy
void backward_model(model m, matrix dL)
{
    matrix d = copy_matrix(dL);
    int i;
    for(i = m.n-1; i >= 0; --i){
        matrix prev = backward_layer(m.layers + i, d);
        free_matrix(d);
        d = prev;
    }
    free_matrix(d);
}

// Update the model weights
// model m: model to update
// double rate: learning rate
// double momentum: amount of momentum to use
// double decay: value for weight decay
void update_model(model m, double rate, double momentum, double decay)
{
    int i;
    for(i = 0; i < m.n; ++i){
        update_layer(m.layers + i, rate, momentum, decay);
    }
}

// Find the index of the maximum element in an array
// double *a: array
// int n: size of a, |a|
// returns: index of maximum element
int max_index(double *a, int n)
{
    if(n <= 0) return -1;
    int i;
    int max_i = 0;
    double max = a[0];
    for (i = 1; i < n; ++i) {
        if (a[i] > max){
            max = a[i];
            max_i = i;
        }
    }
    return max_i;
}

// Calculate the accuracy of a model on some data d
// model m: model to run
// data d: data to run on
// returns: accuracy, number correct / total
double accuracy_model(model m, data d)
{
    matrix p = forward_model(m, d.X);
    int i;
    int correct = 0;
    for(i = 0; i < d.y.rows; ++i){
        if(max_index(d.y.data[i], d.y.cols) == max_index(p.data[i], p.cols)) ++correct;
    }
    return (double)correct / d.y.rows;
}

// Calculate the cross-entropy loss for a set of predictions
// matrix y: the correct values
// matrix p: the predictions
// returns: average cross-entropy loss over data points, 1/n Σ(-ylog(p))
double cross_entropy_loss(matrix y, matrix p)
{
    int i, j;
    double sum = 0;
    for(i = 0; i < y.rows; ++i){
        for(j = 0; j < y.cols; ++j){
            sum += -y.data[i][j]*log(p.data[i][j]);
        }
    }
    return sum/y.rows;
}


// Train a model on a dataset using SGD
// model m: model to train
// data d: dataset to train on
// int batch: batch size for SGD
// int iters: number of iterations of SGD to run (i.e. how many batches)
// double rate: learning rate
// double momentum: momentum
// double decay: weight decay
void train_model(model m, data d, int batch, int iters, double rate, double momentum, double decay)
{
    int e;
    for(e = 0; e < iters; ++e){
        data b = random_batch(d, batch);
        matrix p = forward_model(m, b.X);
        fprintf(stderr, "%06d: Loss: %f\n", e, cross_entropy_loss(b.y, p));
        matrix dL = axpy_matrix(-1, p, b.y); // partial derivative of loss dL/dy
        backward_model(m, dL);
        update_model(m, rate/batch, momentum, decay);
        free_matrix(dL);
        free_data(b);
    }
}


// Questions 
//
// 5.2.2.1 Why might we be interested in both training accuracy and testing accuracy? What do these two numbers tell us about our current model?
// TODONE
// Training accuracy tells us how well our model is doing on the training dataset.
// Testing accuracy tells us how accurate our model is when run on datasets other than the ones in the training dataset.
// Testing accuracy is important since it tells us how well our model generalizes, on data it has never seen before.
// Training accuracy is important for training our model, we want to maximize this accuracy while training.

// 5.2.2.2 Try varying the model parameter for learning rate to different powers of 10 (i.e. 10^1, 10^0, 10^-1, 10^-2, 10^-3) and training the model. What patterns do you see and how does the choice of learning rate affect both the loss during training and the final model accuracy?
// TODONE
// as we increased the learning rate, we briefly increased in final accuracy before going down in final accuracy. as the learning rate was lowered, we only had a decrease in accuracy.
// as the accuracy increased, our loss decreased. The only exception being a learning rate of 10 which had a loss of -nan.
// Our numbers for varying learning rates are below

// rate | training | testing  | loss
// 10:     .0987       .098      -nan
// 1:      .8929       .8889     0.75 ish 
// .1:     .9179       .916      0.3  ish
// .01:    .902        .907      0.4  ish
// .001:   .8586       .869      0.6  ish 
// .0001:  .7262       .7355     1.5  ish
// .00001: .1710       .1678     2.2  ish

// 5.2.2.3 Try varying the parameter for weight decay to different powers of 10: (10^0, 10^-1, 10^-2, 10^-3, 10^-4, 10^-5). How does weight decay affect the final model training and test accuracy?
// TODONE
// The higher the decay, the less accurate model. However, once the decay is low enough, making it smaller seems to have little to no effect on accuracy.

// decay | training | testing
// 1:      .896        .904
// .1:     .901        .907  
// .01:    .902        .907     
// .001:   .902        .907  
// .0001:  .902        .907  
// .00001: .902        .907

// 5.2.3.1 Currently the model uses a logistic activation for the first layer. Try using a the different activation functions we programmed. How well do they perform? What's best?
// TODONE
// We tried LOGISTIC, RELU, LRELU, and SOFT_MAX. From worst to best, it went SOFT_MAX, LOGISTIC, LRELU and then RELU. with about a .8 gap between LOGISTIC and SOFT_MAX and much smaller gaps elsewhere.
// Our data is shown below:

// activation | training | testing
// LOGISTIC:     .8874       .8932
// RELU:         .9228       .9238
// LRELU:        .9206       .9217     
// SOFT_MAX:     .0987       .098  

// 5.2.3.2 Using the same activation, find the best (power of 10) learning rate for your model. What is the training accuracy and testing accuracy?
// the best learning rate was .1 and had training accuracy .9600, and testing accuracy .9542. Data is shown below.

// rate | training | testing  
// 10:     .0987       .098   
// 1:      .0987       .098
// .1:     .9611       .9564   
// .01:    .9228       .9238   
// .001:   .8643       .8681   
// .0001:  .5924       .5915 
// .00001: .1899       .1935 

// 5.2.3.3 Right now the regularization parameter `decay` is set to 0. Try adding some decay to your model. What happens, does it help? Why or why not may this be?
// TODONE
// adding decay (set to .01) lead to a slight decrease in both training (-.0011) and testing (-.0012) accuracy. This did not help, and likely happend due to the fact that the
// model was already very accurate, and increasing decay made it lose some of the progress it had made.

// 5.2.3.4 Modify your model so it has 3 layers instead of two. The layers should be `inputs -> 64`, `64 -> 32`, and `32 -> outputs`. Also modify your model to train for 3000 iterations instead of 1000. Look at the training and testing error for different values of decay (powers of 10, 10^-4 -> 10^0). Which is best? Why?
// TODONE
// the smallest decay actually performed the best. It is really close though. As the decay increases, the training accuracy goes down, but the testing accuracy fluctuates a bit.
// The fluctuation in testing accuracy may have some relation to how the model may be overfitting to the training data, which can lower testing accuracty.

// decay | training | testing
// 1:      .9465       .9433
// .1:     .97785      .9725  
// .01:    .9822       .9706     
// .001:   .9832       .9712  
// .0001:  .9866       .9734  

// 5.3.2.1 How well does your network perform on the CIFAR dataset?
// TODONE
// Cifar is a lot harder than mnist. It takes longer to train, and got a training accuracy of 0.4963 and a testing accuracy of 0.4742.
// We ran this with a 3 layer neural net as described in 5.2.3.4 with the first two layers being RELu.
// We set the learning rate to .01, iters to 5000, momentum to .9 and decay to .1




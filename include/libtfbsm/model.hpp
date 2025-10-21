#ifndef TFBSM_MODEL_HPP
#define TFBSM_MODEL_HPP

namespace tfbsm {

class Model {
   private:
    /* data */
    int test_field;

   public:
    Model(int test_arg = 123) : test_field(test_arg) {}

    void do_something();
};

}  // namespace tfbsm

#endif  // TFBSM_MODEL_HPP

# Robust test for randomness

There are many tests for randomness that are used to test the hardware random generators. The common problem with these tests is the interpretation of results: they often report some "p-values" leaving the interpretation to the user. However, these p-values are not real since they are based on some approximation assumptions (often false). Here we implement the other scheme of testing that provides correct p-values described in [HAL archive reference](https://hal.archives-ouvertes.fr/lirmm-03065320/)

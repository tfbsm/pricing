package util

func Ptr[T any](val T) *T {
	return &val
}

func Val[T any](ptr *T) T {
	var val T

	if ptr != nil {
		val = *ptr
	}

	return val
}

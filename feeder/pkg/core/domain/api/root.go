package api

import (
	"encoding/json"
	"fmt"
	"net/http"
)

const (
	OkMessage    = "ok"
	ErrorMessage = "error"
)

type Reply[T any] struct {
	Code    int    `json:"-"`
	Message string `json:"message,omitempty"`
	Data    *T     `json:"data,omitempty"`
	Error   string `json:"error,omitempty"`
}

func (r Reply[T]) Write(w http.ResponseWriter) error {
	w.Header().Add("Content-Type", "application/json")

	if r.Code == 0 {
		w.WriteHeader(http.StatusOK)
	} else {
		w.WriteHeader(r.Code)
	}

	encoder := json.NewEncoder(w)

	if err := encoder.Encode(r); err != nil {
		return fmt.Errorf("encode json to http response: %w", err)
	}

	return nil
}

func NewError(code int, err error) Reply[any] {
	return Reply[any]{
		Code:    code,
		Message: ErrorMessage,
		Error:   err.Error(),
	}
}

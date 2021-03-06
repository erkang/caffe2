## @package sampling_train
# Module caffe2.python.layers.sampling_train
from __future__ import absolute_import
from __future__ import division
from __future__ import print_function
from __future__ import unicode_literals

from caffe2.python import schema
from caffe2.python.layers.layers import ModelLayer, get_layer_class
from caffe2.python.layers.sampling_trainable_mixin import SamplingTrainableMixin


class SamplingTrain(ModelLayer):
    def __init__(
        self,
        model,
        input_record,
        prediction_layer,
        output_dims,
        name='sampling_train',
        **kwargs
    ):
        super(SamplingTrain, self).__init__(
            model, name, input_record, **kwargs
        )

        layer_class = get_layer_class(prediction_layer)
        assert issubclass(layer_class, SamplingTrainableMixin)

        assert schema.is_schema_subset(
            schema.Struct(
                ('indices', schema.Scalar()),
                ('input', schema.Scalar()),
            ),
            input_record
        )

        self._prediction_layer = layer_class(
            model,
            input_record.input,
            output_dims=output_dims,
            **kwargs
        )

        self._prediction_layer.train_param_blobs = [
            model.net.NextBlob(str(blob) + '_sampled')
            for blob in self._prediction_layer.param_blobs
        ]

        self.params = self._prediction_layer.params

        self.output_schema = self._prediction_layer.output_schema

    def add_ops(self, net):
        self._prediction_layer.add_ops(net)

    def add_train_ops(self, net):
        for full_blob, sampled_blob in zip(
            self._prediction_layer.param_blobs,
            self._prediction_layer.train_param_blobs
        ):
            net.Gather([full_blob, self.input_record.indices()], sampled_blob)
        self._prediction_layer.add_train_ops(net)

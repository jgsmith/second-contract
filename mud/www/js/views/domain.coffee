class SC.Views.Domain extends Backbone.View
  initialize: ->
    @template = _.template $('#domain-template').html()
    @listenTo @model, 'change', @render

  events:
    'click .save': 'save'

  render: ->
    json = @model.toJSON()
    @$el.html @template json
    areaList = new SC.Views.AreaList
      model: @model.areas
      domain: @model.get('name')
    @$el.find("#domain-areas").html areaList.render()
    @el

  save: (e) ->
    e.preventDefault()
    data =
      name: @$('input[name="name"]').val()
      description: @$('input[name="description"]').val()
      plan: @$('textarea[name="plan"]').val()
    @model.set data

    @model.save()

    false

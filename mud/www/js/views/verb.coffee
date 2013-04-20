class SC.Views.Verb extends Backbone.View
  initialize: ->
    @template = _.template $('#verb-template').html()
    @listenTo @model, 'change', @render

  events:
    'click .save': 'save'

  render: ->
    json = @model.toJSON()
    @$el.html @template json
    if @model.isNew()
      @$el.find("h1").text("New Verb")
    # manage settings that are difficult to put into the templates
    if json.actor?.position?
      for pos in json.actor.position
        @$el.find("input[type='checkbox'][name='actor.position'][value='#{pos}']").prop {checked: true}
    if json.actor?.sense?
      for pos in json.actor.sense
        @$el.find("input[type='checkbox'][name='actor.sense'][value='#{pos}']").prop {checked: true}
    if json.actor?.act?
      for pos in json.actor.act
        @$el.find("input[type='checkbox'][name='actor.action'][value='#{pos}']").prop {checked: true}

    for pos in ["direct", "indirect", "instrumental"]
      do (pos) =>
        posEl = @$el.find("input[type='checkbox'][name='arguments'][value='#{pos}']")
        if json.arguments?[pos]?.requirements?
          for req in json.arguments[pos].requirements
            @$el.find("input[type='checkbox'][name='arguments.#{pos}'][value='#{req}']").prop
              checked: true
        $(posEl).change ->
          if $(posEl).is(':checked')
            $(posEl).parent().parent().parent().find(".fieldset-body").fadeIn()
          else
            $(posEl).parent().parent().parent().find(".fieldset-body").fadeOut()

        if json.arguments?[pos]?.used
          posEl.prop
            checked: true
          later ->
            $(posEl).parent().parent().parent().find(".fieldset-body").fadeIn()
        else
          later ->
            $(posEl).parent().parent().parent().find(".fieldset-body").fadeOut()
    @el

  save: (e) ->
    e.preventDefault()
    data =
      verbs: (@$('input[name="verbs"]').val() + "").split(/\s*,\s*/)
      brief: @$('input[name="brief"]').val()
      help: @$('textarea[name="help"]').val()
      see_also: (@$('input[name="see-also"]').val() + "").split(/\s*,\s*/)
      disabled: @$('input[name="disabled"]').is(":checked")
      action: (@$('textarea[name="action"]').val() + "").split(/\s*\n\s*/)
      environment:
        respiration: ($(e).val() for e in @$('input[name="respiration"]') when $(e).is(':checked'))
        timing: ($(e).val() for e in @$('input[name="calendar"]') when $(e).is(':checked'))
        terrain: ($(e).val() for e in @$('input[name="terrain"]') when $(e).is(':checked'))
        weather: ($(e).val() for e in @$('input[name="weather"]') when $(e).is(':checked'))
      arguments:
        direct:
          used: @$('input[name="arguments"][value="direct"]').is(":checked")
          requirements: ($(e).val() for e in @$('input[name="arguments.direct"]') when $(e).is(':checked'))
        indirect:
          used: @$('input[name="arguments"][value="indirect"]').is(":checked")
          requirements: ($(e).val() for e in @$('input[name="arguments.indirect"]') when $(e).is(':checked'))
        instrumental:
          used: @$('input[name="arguments"][value="instrumental"]').is(":checked")
          requirements: ($(e).val() for e in @$('input[name="arguments.instrumental"]') when $(e).is(':checked'))


    @model.set data

    if @model.isNew()
      SC.Collections.verbs.create @model
    else
      @model.save()
 
    false
